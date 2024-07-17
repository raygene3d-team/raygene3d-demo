/*================================================================================
RayGene3D Framework
--------------------------------------------------------------------------------
RayGene3D is licensed under MIT License
================================================================================
The MIT License
--------------------------------------------------------------------------------
Copyright (c) 2021

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
================================================================================*/


#pragma once
#include "../raygene3d-wrap/wrap.h"

namespace RayGene3D
{
  class PhotonmapBroker : public Broker
  {
  protected:
    std::shared_ptr<Property> prop_scene;

  protected:
    std::shared_ptr<Property> prop_instances;
    std::shared_ptr<Property> prop_triangles;
    std::shared_ptr<Property> prop_vertices;

  protected:
    enum Axis
    {
      //AXIS_NONE = 0,
      //AXIS_X = 0b01000000,
      //AXIS_Y = 0b10000000,
      //AXIS_Z = 0b11000000
    };

    struct Photon
    {
      glm::f32vec3 position;
      //glm::u8vec3 color;
      //uint8_t exponent;

      //uint32_t color; //RGBM

			uint32_t axis;
			glm::f32vec3 color;
			uint32_t padding;
    };

		template <typename Node>
		class KDTree
		{
		public:
			struct Entry
			{
				Node* node;
				float d_sq;

				bool operator<(const Entry& entry) const { return d_sq < entry.d_sq; }

				Entry(Node* node = nullptr, float d_sq = 0.0f)
					: node(node)
					, d_sq(d_sq)
				{}
			};

		protected:
			std::vector<Node> nodes;

			static bool LessX(const Node& l, const Node& r) { return l.position.x < r.position.x; }
			static bool LessY(const Node& l, const Node& r) { return l.position.y < r.position.y; }
			static bool LessZ(const Node& l, const Node& r) { return l.position.z < r.position.z; }

			void RecursiveCreate(typename std::vector<Node>::iterator begin,
				typename std::vector<Node>::iterator end, uint32_t depth)
			{
				if (end - begin <= 0) return;

				auto axis = uint32_t(depth % 3);

				auto min = glm::f32vec3{ FLT_MAX };
				auto max = glm::f32vec3{-FLT_MAX };

				for (auto it = begin; it < end; ++it)
				{
					min = glm::min(min, it->position);
					max = glm::max(max, it->position);
				}

				const auto center = 0.5f * (max + min);
				const auto extent = 0.5f * (max - min);

				axis = 0;
				if (extent.y > extent.x || extent.z > extent.x)
				{
					if (extent.z > extent.y)
						axis = 2;
					else
						axis = 1;
				}

				switch (axis)
				{
				case 0:
					std::sort(begin, end, [](const auto& l, const auto& r) { return l.position.x < r.position.x; }); break;
				case 1:
					std::sort(begin, end, [](const auto& l, const auto& r) { return l.position.y < r.position.y; }); break;
				case 2:
					std::sort(begin, end, [](const auto& l, const auto& r) { return l.position.z < r.position.z; }); break;
				}

				const auto median = (end - begin) / 2;
				auto node = &(*(begin + median));
				node->axis = axis;

				RecursiveCreate(begin, begin + median, depth + 1);
				RecursiveCreate(begin + median + 1, end, depth + 1);
			}

			void RecursiveGather(typename std::vector<Node>::iterator begin, typename std::vector<Node>::iterator end,
				const glm::f32vec3& position, const float max_d_sq, std::priority_queue<Entry, std::vector<Entry>>& queue)
			{
				if (end - begin <= 0) return;

				const auto median = (end - begin) / 2;
				auto node = &(*(begin + median));

				auto delta = 0.0f;
				switch (node->axis)
				{
				case 0:
					delta = position.x - node->position.x; break;
				case 1:
					delta = position.y - node->position.y; break;
				case 2:
					delta = position.z - node->position.z; break;
				}

				const auto d_sq = glm::distance2(node->position, position);

				if (d_sq < max_d_sq)
					queue.push(Entry(node, distance_sq));

				if (delta > 0.0f)
				{
					RecursiveGather(begin + median + 1, end, position, max_d_sq, queue);
					if (delta * delta < max_d_sq)
						RecursiveGather(begin, begin + median, position, max_d_sq, queue);
				}
				else
				{
					RecursiveGather(begin, begin + median, position, max_d_sq, queue);
					if (delta * delta < max_d_sq)
						RecursiveGather(begin + median + 1, end, position, max_d_sq, queue);
				}
			}


			void RecursiveNearest(typename std::vector<Node>::iterator begin, typename std::vector<Node>::iterator end,
				const glm::f32vec3& position, Entry& entry)
			{
				if (end - begin <= 0) return;

				const auto median = (end - begin) / 2;
				auto node = &(*(begin + median));

				auto delta = 0.0f;
				switch (node->axis)
				{
				case 0:
					delta = position.x - node->position.x; break;
				case 1:
					delta = position.y - node->position.y; break;
				case 2:
					delta = position.z - node->position.z; break;
				}

				const auto d_sq = glm::distance2(node->position, position);


				if (d_sq < entry.d_sq)
				{
					entry.node = node;
					entry.d_sq = d_sq;
				}

				if (delta > 0.0f)
				{
					RecursiveNearest(begin + median + 1, end, position, entry);
					if (delta * delta < entry.d_sq)
						RecursiveNearest(begin, begin + median, position, entry);
				}
				else
				{
					RecursiveNearest(begin, begin + median, position, entry);
					if (delta * delta < entry.d_sq)
						RecursiveNearest(begin + median + 1, end, position, entry);
				}
			}

			void RecursiveNearest(typename std::vector<Node>::iterator begin, typename std::vector<Node>::iterator end,
				const glm::f32vec3& position, const glm::f32vec3& normal, Entry& entry)
			{
				if (end - begin <= 0) return;

				const auto median = (end - begin) / 2;
				auto node = &(*(begin + median));

				auto delta = 0.0f;
				switch (node->axis)
				{
				case 0:
					delta = position.x - node->position.x; break;
				case 1:
					delta = position.y - node->position.y; break;
				case 2:
					delta = position.z - node->position.z; break;
				}

				const auto d_sq = glm::distance2(node->position, position);


				if (d_sq < entry.d_sq)
				{
					if (Dot(normal, node->normal) < 0.0f)
					{
						entry.node = node;
						entry.d_sq = d_sq;
					}
				}

				if (delta > 0.0f)
				{
					RecursiveNearest(begin + median + 1, end, position, entry);
					if (delta * delta < entry.d_sq)
						RecursiveNearest(begin, begin + median, position, entry);
				}
				else
				{
					RecursiveNearest(begin, begin + median, position, entry);
					if (delta * delta < entry.d_sq)
						RecursiveNearest(begin + median + 1, end, position, entry);
				}
			}

		public:
			std::vector<Node>& GetNodes()
			{
				return nodes;
			}

			void Gather(const glm::f32vec3& position, const float max_d_sq, std::priority_queue<Entry, std::vector<Entry>>& queue)
			{
				RecursiveGather(nodes.begin(), nodes.end(), position, max_d_sq, queue);
			}

			void Nearest(const glm::f32vec3& position, Entry& entry)
			{ 
				RecursiveNearest(nodes.begin(), nodes.end(), position, entry);
			}

			void Nearest(const glm::f32vec3& position, const glm::f32vec3& normal, Entry& entry)
			{ 
				RecursiveNearest(nodes.begin(), nodes.end(), position, normal, entry);
			}

			void Create()
			{ 
				RecursiveCreate(nodes.begin(), nodes.end(), 0);
			}
		};

  public:
    void Initialize() override;
    void Use() override;
    void Discard() override;

  public:
    PhotonmapBroker(Wrap& wrap);
    virtual ~PhotonmapBroker();
  };
}
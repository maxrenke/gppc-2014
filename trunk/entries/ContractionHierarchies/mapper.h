#ifndef MAPPER_H
#define MAPPER_H
#include <vector>
#include <cstdint>
#include "Entry.h"
#include "arc.h"

inline
std::vector<int>inverse_permutation(const std::vector<int>&order){
	std::vector<int>inv_order(order.size());
	for(int i=0; i<(int)order.size(); ++i)
		inv_order[order[i]] = i;
	return std::move(inv_order);
}

class Mapper{
public:
	Mapper(){}
	Mapper(const std::vector<bool>&field, int width, int height):
		width_(width), 
		height_(height),
		node_count_(0),
		pos_to_node_(width*height, -1)
	{
		for(int y=0; y<height_; ++y)
			for(int x=0; x<width_; ++x)
				if(field[x+y*width_]){
					node_to_pos_.push_back({static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)});
					pos_to_node_[x+y*width_] = node_count_++;
				}else{
					pos_to_node_[x+y*width_] = -1;
				}
	}

	int width()const{
		return width_;
	}

	int height()const{
		return height_;
	}

	int node_count()const{
		return node_count_;
	}	

	xyLoc operator()(int x)const{ return node_to_pos_[x]; }
	int operator()(xyLoc p)const{ 
		if(p.x < 0 || p.x >= width_ || p.y < 0 || p.y >= height_)
			return -1;
		else
			return pos_to_node_[p.x + p.y*width_];
	}

	int operator()(int x, int y)const{
		return (*this)({static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)});
	}

	void reorder(const std::vector<int>&perm, const std::vector<int>&inv_perm){
		for(auto&x:pos_to_node_){
			if(x != -1){
				x = inv_perm[x];
			}
		}
		std::vector<xyLoc>new_node_to_pos_(node_count_);
		for(int new_node=0; new_node<node_count(); ++new_node){
			int old_node = perm[new_node];
			new_node_to_pos_[new_node] = node_to_pos_[old_node];
		}
		new_node_to_pos_.swap(node_to_pos_);
	}
private:
	int width_, height_, node_count_;
	std::vector<int>pos_to_node_;
	std::vector<xyLoc>node_to_pos_;
};

inline
std::vector<WeightedArc> extract_graph(const Mapper&mapper){
	std::int16_t dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
	std::int16_t dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
	int dw[] = {1414, 1000, 1414, 1000, 1000, 1414, 1000, 1414}; 
	std::vector<WeightedArc>arc_list;
	for(int u=0; u<mapper.node_count(); ++u){
		auto u_pos = mapper(u);
		for(int d = 0; d<8; ++d){
			xyLoc v_pos = {static_cast<std::int16_t>(u_pos.x + dx[d]), static_cast<std::int16_t>(u_pos.y + dy[d])};
			int v = mapper(v_pos);
			if(v != -1 && mapper(xyLoc{u_pos.x, static_cast<std::int16_t>(u_pos.y+dy[d])}) != -1 &&  mapper(xyLoc{static_cast<std::int16_t>(u_pos.x+dx[d]), u_pos.y}) != -1)
				arc_list.push_back({u, v, dw[d]});
		}
	}
	return arc_list;
}

void dump_map(const Mapper&map, const char*file);

#endif


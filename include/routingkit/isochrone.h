#ifndef ISOCHRONE_H
#define ISOCHRONE_H

#include <routingkit/id_queue.h>
#include <routingkit/constants.h>
#include <vector>

namespace RoutingKit{

class Isochrone{
public:
	Isochrone():first_out(nullptr){}

	Isochrone(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, const std::vector<unsigned>&weight):
		was_popped(first_out.size()-1),
		queue(first_out.size()-1),
		first_out(&first_out),
		head(&head),
		weight(&weight) {
		assert(!first_out.empty());
		assert(first_out.front() == 0);
		assert(first_out.back() == head.size());
		assert(first_out.back() == weight.size());
	}

	Isochrone&reset(){
		queue.clear();
		std::fill(was_popped.begin(), was_popped.end(), false);
		return *this;
	}

	Isochrone&reset(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, const std::vector<unsigned>&weight){
		assert(!first_out.empty());
		assert(first_out.front() == 0);
		assert(first_out.back() == head.size());
		assert(first_out.back() == weight.size());

		if(this->first_out != nullptr && first_out.size() == this->first_out->size()){
			this->first_out = &first_out;
			this->head = &head;
			this->weight = &weight;
			queue.clear();
			std::fill(was_popped.begin(), was_popped.end(), false);
			return *this;
		}else{
			this->first_out = &first_out;
			this->head = &head;
			this->weight = &weight;
			was_popped = std::vector<bool>(first_out.size()-1);
			queue = MinIDQueue(first_out.size()-1);
			return *this;
		}
	}

	Isochrone&add_source(unsigned id, unsigned time_limit, unsigned departure_time = 0){
		assert(id < first_out->size()-1);
        assert(time_limit > 0);
        this->time_limit = time_limit;
		queue.push({id, departure_time});
		return *this;
	}

	bool is_finished()const{
		return queue.empty();
	}

	bool was_node_reached(unsigned x)const{
		assert(x < first_out->size()-1);
		return was_popped[x];
	}

	unsigned settle(){
		assert(!is_finished());

		auto p = queue.pop();
		was_popped[p.id] = true;
		if (p.key > time_limit) {
			return p.id;
		}

		for(unsigned a=(*first_out)[p.id]; a<(*first_out)[p.id+1]; ++a){
			if(!was_popped[(*head)[a]]){
				unsigned w = (*weight)[a];
				if(w < inf_weight){
					if(queue.contains_id((*head)[a])){
						queue.decrease_key({(*head)[a], p.key + w});
					} else {
						queue.push({(*head)[a], p.key + w});
					}
				}
			}
		}
		return p.id;
	}

private:
    unsigned time_limit;
	std::vector<bool> was_popped;
	MinIDQueue queue;

	const std::vector<unsigned>*first_out;
	const std::vector<unsigned>*head;
	const std::vector<unsigned>*weight;
};

}
#endif

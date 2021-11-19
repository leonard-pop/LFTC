#include "IteratorMDO.h"
#include "MDO.h"

// O(h)
IteratorMDO::IteratorMDO(const MDO& d) : dict(d){
	this->prim();
}

// O(h)
void IteratorMDO::parcurgePeStanga(Nod *start){
	this->curent = start;

	if(this->curent == nullptr){
		return;
	}

	while(this->curent->stanga != nullptr){
		this->curent = this->curent->stanga;
	}
	this->index = 0;
}

// O(h)
void IteratorMDO::prim(){
	this->parcurgePeStanga(this->dict.radacina);
}

// O(h)
void IteratorMDO::urmator(){
	if(!this->valid()){
		throw exception();
	}

	if(this->index < this->curent->dimensiune_valori){
		this->index++;
	} else{
		this->index = 0;

		if(this->curent->dreapta != nullptr){
			this->parcurgePeStanga(this->curent->dreapta);
		} else{
			while(this->curent->parinte != nullptr &&
				this->curent->parinte->stanga != this->curent){

				this->curent = this->curent->parinte;
			}
			this->curent = this->curent->parinte;
		}
	}
}

// Theta(1)
bool IteratorMDO::valid() const{
	return this->curent != nullptr;
}

// Theta(1)
TElem IteratorMDO::element() const{
	if(!this->valid()){
		throw exception();
	}

	return make_pair(this->curent->cheie,
		this->curent->valori[index]);
}

#include "IteratorMDO.h"
#include "MDO.h"
#include <iostream>
#include <vector>
#include <stack>

#include <exception>

#define CAPACITATE_INITIALA 2

using namespace std;

// Theta(1)
Nod::Nod(TCheie cheie, TValoare valoare, Nod *parinte): cheie{cheie},
	parinte{parinte}{

	this->valori = new TValoare[CAPACITATE_INITIALA];
	this->capacitate_valori = CAPACITATE_INITIALA;
	
	this->valori[0] = valoare;
	this->dimensiune_valori = 1;
}

// Theta(1)
Nod::~Nod(){
	delete[] this->valori;
}

// Theta(1) amortizat
void Nod::adauga(TValoare valoare){
	if(this->dimensiune_valori == this->capacitate_valori){
		int capacitate_noua = this->capacitate_valori * 2;
		TValoare *valori_noi = new TValoare[capacitate_noua];

		for(int i = 0; i < this->dimensiune_valori; i++){
			valori_noi[i] = this->valori[i];
		}

		delete[] this->valori;
		this->valori = valori_noi;
		this->capacitate_valori = capacitate_noua;
	}

	this->valori[this->dimensiune_valori] = valoare;
	this->dimensiune_valori++;
}

// O(n)
void Nod::sterge(int pozitie){
	if(pozitie < 0 || pozitie >= this->dimensiune_valori){
		return;
	}

	for(int i = pozitie; i < this->dimensiune_valori - 1; i++){
		this->valori[i] = this->valori[i + 1];
	}

	this->dimensiune_valori--;
}

// Theta(1)
MDO::MDO(Relatie r) {
	this->relatie = r;
}

// Theta(n)
vector<TCheie> MDO::multimeaCheilor() const{
	vector<TCheie> chei;

	stack<Nod*> de_explorat;
	Nod *curent = nullptr;

	if(this->radacina != nullptr){
		de_explorat.push(this->radacina);
	}

	while(!de_explorat.empty()){
		curent = de_explorat.top();
		de_explorat.pop();

		if(curent->stanga != nullptr){
			de_explorat.push(curent->stanga);
		}
		if(curent->dreapta != nullptr){
			de_explorat.push(curent->dreapta);
		}

		chei.push_back(curent->cheie);
	}

	return chei;
}

// O(h)
void MDO::stergeNod(Nod *nod){
	if(nod->stanga == nullptr && nod->dreapta == nullptr){
		if(nod->parinte != nullptr){
			if(nod->parinte->stanga == nod){
				nod->parinte->stanga = nullptr;
			} else{
				nod->parinte->dreapta = nullptr;
			}
		} else{
			this->radacina = nullptr;
		}

		delete nod;
	} else if(nod->stanga == nullptr){
		nod->dreapta->parinte = nod->parinte;
		if(nod->parinte != nullptr){
			if(nod->parinte->stanga == nod){
				nod->parinte->stanga = nod->dreapta;
			} else{
				nod->parinte->dreapta = nod->dreapta;
			}
		} else{
			this->radacina = nod->dreapta;
		}

		delete nod;
	} else if(nod->dreapta == nullptr){
		nod->stanga->parinte = nod->parinte;
		if(nod->parinte != nullptr){
			if(nod->parinte->stanga == nod){
				nod->parinte->stanga = nod->stanga;
			} else{
				nod->parinte->dreapta = nod->stanga;
			}
		} else{
			this->radacina = nod->stanga;
		}

		delete nod;
	} else{
		Nod *minim = this->cautaNodMinim(nod->dreapta);
		cout << nod->cheie << " <- " << minim->cheie << '\n';

		nod->cheie = minim->cheie;

		delete[] nod->valori;
		nod->valori = new TValoare[minim->dimensiune_valori];
		for(int i = 0; i < minim->dimensiune_valori; i++){
			nod->valori[i] = minim->valori[i];
		}
		nod->dimensiune_valori = minim->dimensiune_valori;
		nod->capacitate_valori = minim->dimensiune_valori;

		stergeNod(minim);
	}
}

// O(h)
Nod* MDO::cautaNodMinim(Nod *start){
	if(start == nullptr){
		return nullptr;
	}

	Nod *curent = start;

	while(curent->stanga != nullptr){
		curent = curent->stanga;
	}

	return curent;
}

// O(h) amortizat
void MDO::adauga(TCheie cheie, TValoare valoare) {
	if(this->radacina == nullptr){
		this->radacina = new Nod(cheie, valoare, nullptr);
	} else{
		Nod *candidat = this->radacina,
			*precedent = nullptr;
		bool ultima_directie = 0;

		while(candidat != nullptr && candidat->cheie != cheie){
			precedent = candidat;

			if(this->relatie(cheie, candidat->cheie) == true){
				candidat = candidat->stanga;
				ultima_directie = 0;
			} else{
				candidat = candidat->dreapta;
				ultima_directie = 1;
			}
		}

		if(candidat == nullptr){
			if(ultima_directie == 0){
				precedent->stanga = new
					Nod(cheie, valoare, precedent);
			} else{
				precedent->dreapta = new
					Nod(cheie, valoare, precedent);
			}
		} else{
			candidat->adauga(valoare);
		}
	}

	this->dimensiune++;
}

// O(h)
vector<TValoare> MDO::cauta(TCheie cheie) const {
	vector<TValoare> valori_gasite;
	Nod *candidat = this->radacina;

	while(candidat != nullptr && candidat->cheie != cheie){
		if(this->relatie(cheie, candidat->cheie) == true){
			candidat = candidat->stanga;
		} else{
			candidat = candidat->dreapta;
		}
	}

	if(candidat != nullptr){
		for(int i = 0; i < candidat->dimensiune_valori; i++){
			valori_gasite.push_back(candidat->valori[i]);
		}
	}

	return valori_gasite;
}

// O(n)
bool MDO::sterge(TCheie cheie, TValoare valoare) {
	Nod *candidat = this->radacina;

	while(candidat != nullptr && candidat->cheie != cheie){
		if(this->relatie(cheie, candidat->cheie) == true){
			candidat = candidat->stanga;
		} else{
			candidat = candidat->dreapta;
		}
	}

	if(candidat != nullptr){
		for(int i = 0; i < candidat->dimensiune_valori; i++){
			if(candidat->valori[i] == valoare){
				candidat->sterge(i);

				if(candidat->dimensiune_valori == 0){
					this->stergeNod(candidat);
				}

				this->dimensiune--;
				return true;
			}
		}
	}

	return false;
}

// Theta(1)
int MDO::dim() const {
	return this->dimensiune;
}

// Theta(1)
bool MDO::vid() const {
	return this->dimensiune == 0;
}

// Theta(1)
IteratorMDO MDO::iterator() const {
	return IteratorMDO(*this);
}

// Theta(n)
MDO::~MDO() {
	stack<Nod*> de_sters;
	Nod *curent = nullptr;

	if(this->radacina != nullptr){
		de_sters.push(this->radacina);
	}

	while(!de_sters.empty()){
		curent = de_sters.top();
		de_sters.pop();

		if(curent->stanga != nullptr){
			de_sters.push(curent->stanga);
		}
		if(curent->dreapta != nullptr){
			de_sters.push(curent->dreapta);
		}

		delete curent;
	}
}

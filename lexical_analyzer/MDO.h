#pragma once

#include <vector>
#include <string>
#include <utility>

using namespace std;

typedef string TCheie;
typedef int TValoare;
typedef pair<TCheie, TValoare> TElem;

class IteratorMDO;

typedef bool(*Relatie)(TCheie, TCheie);

class Nod {
	public:
		TCheie cheie;
		TValoare *valori;
		int dimensiune_valori, capacitate_valori;
		Nod *stanga = nullptr,
			*dreapta = nullptr,
			*parinte;

		Nod(TCheie cheie, TValoare valoare, Nod *parinte);
		~Nod();
		void adauga(TValoare valoare);
		void sterge(int pozitie);
};

class MDO {
	friend class IteratorMDO;
    private:
		Relatie relatie;
		Nod *radacina = nullptr;
		int dimensiune = 0;

		void stergeNod(Nod *nod);
		Nod* cautaNodMinim(Nod *start);

    public:

	// constructorul implicit al MultiDictionarului Ordonat
	MDO(Relatie r);

	vector<TCheie> multimeaCheilor() const;

	// adauga o pereche (cheie, valoare) in MDO
	void adauga(TCheie c, TValoare v);

	//cauta o cheie si returneaza vectorul de valori asociate
	vector<TValoare> cauta(TCheie c) const;

	//sterge o cheie si o valoare 
	//returneaza adevarat daca s-a gasit cheia si valoarea de sters
	bool sterge(TCheie c, TValoare v);

	//returneaza numarul de perechi (cheie, valoare) din MDO 
	int dim() const;

	//verifica daca MultiDictionarul Ordonat e vid 
	bool vid() const;

	// se returneaza iterator pe MDO
	// iteratorul va returna perechile in ordine in raport cu relatia de ordine
	IteratorMDO iterator() const;

	// destructorul 	
	~MDO();

};

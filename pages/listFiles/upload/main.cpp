/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elrichar <elrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 15:47:02 by elrichar          #+#    #+#             */
/*   Updated: 2024/04/18 16:17:09 by elrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MateriaSource.hpp"
#include "Character.hpp"
#include "Ice.hpp"
#include "Cure.hpp"

int main()
{

{
	IMateriaSource* src = new MateriaSource();
	src->learnMateria(new Ice());
	src->learnMateria(new Cure());
	ICharacter* me = new Character("me");
	AMateria* tmp;
	tmp = src->createMateria("ice");
	me->equip(tmp);
	tmp = src->createMateria("cure");
	me->equip(tmp);
	ICharacter* bob = new Character("bob");
	me->use(0, *bob);
	me->use(1, *bob);
	delete bob;
	delete me;
	delete src;
	std::cout << "=========================================" << std::endl;
	std::cout << "=========================================" << std::endl;
}

{
	IMateriaSource *src = new MateriaSource();

	AMateria *test = new Cure();
	//learning new materias
	src->learnMateria(test);
	src->learnMateria(new Ice());
	src->learnMateria(test);
	src->learnMateria(new Cure());
	src->learnMateria(new Ice());

	//inventory full
	src->learnMateria(new Ice());
	
	std::cout << std::endl;

	Character *Elo = new Character;
	AMateria *materia;
	
	//let's equip 
	//wrong materia
	materia = src->createMateria("i");
	Elo->equip(materia);
	
	//correct materia
	materia = src->createMateria("ice");
	Elo->equip(materia);
	materia = src->createMateria("ice");
	Elo->equip(materia);
	materia = src->createMateria("cure");
	Elo->equip(materia);
	materia = src->createMateria("cure");
	Elo->equip(materia);

	//use + copy assignment operator
	Character *Copy_operator = new Character();
	*Copy_operator = *Elo;
	Elo->use(1, *Copy_operator);
	Elo->use(18, *Copy_operator);
	Elo->use(2, *Copy_operator);
	Elo->use(3, *Copy_operator);
	Elo->use(0, *Copy_operator);
	Copy_operator->use(0, *Elo);
	

	//inventory full
	materia = src->createMateria("ice");
	Elo->equip(materia);
	delete materia;

	//copy constructor
	Character *Copy_constructor = new Character(*Elo);
	Copy_constructor->use(0, *Elo);
	Copy_constructor->use(1, *Copy_operator);
	
	//unequip
	//incorrect index
	Elo->unequip(17);
	Elo->unequip(-70);

	//correct indexes
	materia = Elo->leaveMateria(0);
	delete materia;
	Elo->unequip(0);
	materia = Elo->leaveMateria(1);
	delete materia;
	Elo->unequip(1);
	materia = Elo->leaveMateria(2);
	delete materia;
	Elo->unequip(2);
	
	materia = Copy_operator->leaveMateria(0);
	delete materia;
	Copy_operator->unequip(0);

	delete Elo;
	delete Copy_operator;
	delete Copy_constructor;
	delete src;
	// // return (0);
}

}

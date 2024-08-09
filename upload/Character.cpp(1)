/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elrichar <elrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 18:55:27 by elrichar          #+#    #+#             */
/*   Updated: 2024/04/17 18:21:16 by elrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character.hpp"

Character::Character(void) : _name("no name")
{
	// std::cout << "[Character] : default constructor called" << std::endl;
	for (int i = 0; i < 4; i++)
		this->_inventory[i] = NULL;
}

Character::Character(std::string name) : _name(name)
{
	// std::cout << "[Character] : parametric constructor called" << std::endl;
	for (int i = 0; i < 4; i++)
		this->_inventory[i] = NULL;
}

std::string const & Character::getName() const
{
	return (this->_name);
}

Character::Character(Character const &other)
{
	// std::cout << "[Character] : copy constructor called" << std::endl;

	this->_name = other._name;

	for (int i = 0; i < 4; i++)
	{
		if (other._inventory[i])
			this->_inventory[i] = other._inventory[i]->clone();
		else
			this->_inventory[i] = NULL;
	}
}

Character &Character::operator = (Character const &other)
{
	// std::cout << "[Character] : copy assignment operator called" << std::endl;
	
	if (this != &other)
	{
		this->_name = other._name;
		for (int i = 0; i < 4; i++)
		{
			if (this->_inventory[i])
			{
				delete this->_inventory[i];
				this->_inventory[i] = NULL;
			}
			if (other._inventory[i])
				this->_inventory[i] = other._inventory[i]->clone();
		}
	}
	return (*this);
}

Character::~Character(void)
{
	// std::cout << "[Character] : destructor called" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		 if (this->_inventory[i])
        {
            delete this->_inventory[i];
            this->_inventory[i] = NULL;
        }
	}
}

void Character::equip(AMateria *m)
{
	if (!m)
		return ;
	for (int i = 0; i < 4; i++)
	{
		if (!this->_inventory[i])
		{
			this->_inventory[i] = m;
			// std::cout << m->getType() << " Equipment." << std::endl;
			return ; 
		}
	}
	std::cout << "The inventory is full" << std::endl;
}

void Character::unequip(int idx)
{
	if (idx < 0 || idx > 3)
	{
		std::cout << "Incorrect index" << std::endl;
		return ;
	}
	if (this->_inventory[idx])
		this->_inventory[idx] = NULL;		
	// std::cout << this->getName() << " is now unequiped of " << this->_inventory[idx]->getType();
}

void Character::use(int idx,ICharacter &target)
{
	if (idx < 0 || idx > 3)
	{
		std::cout << "Incorrect index" << std::endl;
		return ;
	}
	if (this->_inventory[idx])
		this->_inventory[idx]->use(target);
	else
		std::cout << "No materia available at index " << idx << std::endl;
}

AMateria *Character::leaveMateria(int idx)
{
	if (idx < 0 || idx > 3)
	{
		std::cout << "Wrong Index" << std::endl;
		return (NULL);
	}
    if (this->_inventory[idx])
	{
        return (this->_inventory[idx]);
	}
	return (NULL);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Zombie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juandrie <juandrie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 10:51:07 by juandrie          #+#    #+#             */
/*   Updated: 2024/03/14 11:08:51 by juandrie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <iostream>
#include <string>

class Zombie
{
private:
   std::string  name;
   
public:
    Zombie();
    Zombie(std:: string name);
    ~Zombie();
    
    void    announce(void) const;
};

Zombie* zombieHorde(int N, std::string name);


#endif 
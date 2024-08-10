/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Zombie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juandrie <juandrie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:42:59 by juandrie          #+#    #+#             */
/*   Updated: 2024/03/04 18:16:03 by juandrie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <iostream>
#include <string>

class Zombie
{
private:

    std:: string name;

public:

    Zombie(std:: string name);
    ~Zombie();
    void announce(void) const;
};

Zombie  *newZombie(std::string name);
void    randomChump(std::string name);

#endif



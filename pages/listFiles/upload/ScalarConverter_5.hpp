/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScalarConverter.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elrichar <elrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 14:29:45 by elrichar          #+#    #+#             */
/*   Updated: 2024/05/09 18:04:11 by elrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCALARCONVERTER_HPP
#define SCALARCONVERTER_HPP

#include <string>
#include <iostream>
#include <limits>
#include <cstdlib>
#include <sstream>

class ScalarConverter
{
	public:
	static void convert(std::string str);
	
	private:
	ScalarConverter(void);
	ScalarConverter (const ScalarConverter &other);
	ScalarConverter &operator = (const ScalarConverter &other);
	~ScalarConverter(void);
	
};

bool is_specific(std::string str);
bool is_char(std::string str);
bool is_int(std::string str);
bool is_float(std::string str);
bool is_double(std::string str);




void display_specific(std::string str);
void display_char(std::string str);
void display_int(std::string str);
void display_float(std::string str);
void display_double(std::string str);



void display_error(void);
void print_error(std::string str);

#endif
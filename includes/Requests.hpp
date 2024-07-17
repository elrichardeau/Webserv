/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:57:21 by niromano          #+#    #+#             */
/*   Updated: 2024/07/17 12:25:54 by niromano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

class Requests {

	public :

		Requests(const std::string &method, const std::string &path, const std::string &protocol, std::vector<std::string> &accept);
		~Requests();
		std::string getResponse();

	private :

		const std::string _method;
		const std::string _path;
		const std::string _protocol;
		std::vector<std::string> _accept;
};

Requests readRequest(std::string buf);
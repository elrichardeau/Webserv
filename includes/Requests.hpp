/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:57:21 by niromano          #+#    #+#             */
/*   Updated: 2024/07/18 17:24:26 by niromano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

class Requests {

	public :

		Requests(std::string &statusCode);
		Requests(const std::string &method, const std::string &path, const std::string &protocol, std::vector<std::string> &accept);
		~Requests();
		std::string getResponse();
		std::string getStatusCode();

	private :

		std::string _statusCode;
		const std::string _method;
		const std::string _path;
		const std::string _protocol;
		std::vector<std::string> _extensionAcceptedList;
};

Requests readRequest(std::string buf);
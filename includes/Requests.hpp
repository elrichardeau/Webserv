/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:57:21 by niromano          #+#    #+#             */
/*   Updated: 2024/07/19 16:16:37 by niromano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

enum StatusCode {
    OK = 200,
    BAD_REQUEST = 400,
	FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
    INTERNAL_SERVER_ERROR = 500,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

class Requests {

	public :

		Requests(int statusCode);
		Requests(const std::string &method, const std::string &path, std::vector<std::string> &accept);
		~Requests();
		std::string getResponse();
		std::string getStatusCode();

	private :

		int _statusCode;
		const std::string _method;
		const std::string _path;
		const std::vector<std::string> _accept;
};

Requests readRequest(std::string buf);
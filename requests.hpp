/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requests.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:57:21 by niromano          #+#    #+#             */
/*   Updated: 2024/07/15 17:44:09 by niromano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

class Requests {

	public :

		Requests(const std::string &type, const std::string &path, const std::string &method);
		~Requests();
		std::string getType();
		std::string getPath();
		std::string getMethod();
		std::string getResponse();
		bool isSyntaxError();

	private :

		const std::string _type;
		const std::string _path;
		const std::string _method;
};

Requests readRequest(std::string buf);
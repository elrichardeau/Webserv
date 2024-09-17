# Webserv Project 🌐
Webserv is a **group project** that involves **creating a web server capable of handling HTTP requests and responses according to the HTTP/1.1 protocol**. 
This project helps us deepen our understanding of **network communication protocols** and enhances our programming skills **using C++**.

It is a straightforward yet fully functional implementation of an HTTP server, featuring several capabilities, **including handling GET, POST, and DELETE requests**.

## Features 📋
1. **Request Handling:** Supports HTTP methods like GET, POST, and DELETE.
2. **Connection Management:** Efficient handling of multiple simultaneous client connections.
3. **Non-blocking Server:** The server **must never block when handling requests**, ensuring that all connections are managed efficiently and without delays.
4. **Error Handling:** Generates appropriate HTTP response codes and error pages.
5. **Configurable Server:** The server can be configured via a custom configuration file, defining host, port, and routing rules.
6. **CGI Support:** Executes CGI scripts, enabling dynamic content generation.
7. **Static Content Delivery:** Serves static files such as HTML, CSS, JavaScript, and image files.

## Objectives 🎯
1. Improve skills in network programming using C++.
2. Master the HTTP/1.1 protocol and its nuances.
3. Enhance knowledge of efficient server design and handling of concurrent connections.
4. Learn and apply system calls for socket management and process control.

## Screenshots 🖼️
We start by designing the server configuration and handling requests, with an example configuration file that looks like this:
![Capture d’écran 2024-09-17 131958](https://github.com/user-attachments/assets/1af0340b-5ab0-40be-940f-58b71318bdba)



Here is a screen of our webserv running on the IP and port specified in the configuration file, allowing us to test different options such as POST, GET, and DELETE requests, as well as file uploads:
![Capture d’écran 2024-08-20 202156](https://github.com/user-attachments/assets/bf9bee54-bd59-453c-b599-25d94f9167de)


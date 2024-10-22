# TODO List Project: Desktop front end
- Server: link here:
- Web app: link here

## Description
TODO Desktop is a C++ desktop application that allows users to create and manage to-do lists and tasks. These tasks are stored on a C++ server with a MySQL database, which can be found in the link above. The desktop app communicates with the server via HTTP requests to retrieve, update, and manage the user's to-do lists and tasks.

The project integrates Google OAuth 2.0 for user authentication. Upon login, the app collects only the user's email, first name, and Google ID. These credentials ensure that users can only access their own tasks and lists.

## Features
- Create, update and delete lists and tasks
- HTTP request
- Login Via Google OAuth 2.0

## Libraries 
The project is built using [Walnut](https://github.com/StudioCherno/Walnut) 

- Imgui
- Crow
- curl
- nlohmann json

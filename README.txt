Simple Pattern Matching Game with Balls

URL:
https://github.com/Ffaithy/Simple-Pattern-Matching-Game

Technologies:
The project is developes in C++, compiled with Visual Studio 2013.
Additional libraries: SDL2, SDL2_image (for loading PNGs), SDL2_ttf (for text rendering).

Rules:
* The board is populated with random balls.
* User must match at least 3 balls of same colour.
* The matched balls explode in a fading animation.
* The board is then filled with new balls falling from above.
* User must obtain a certain score using a limited amount of moves.
* The game currently is playable only by Mouse clicking on cells.
* First click selects a cell, second click selects another cell and then, the cells are swapped.

Testing:
* Currently, there are 3 levels available for testing.
* The levels are loaded from text files.
* A level file contains: the types of balls that will populate the board, the objective (a specific score), the number of available moves.
* When user completes a level, data is saved in text tile: level number + score. Next time the user starts the game, the next available level is loaded.
* When user gets out of moves, it has the option to retry the same level.
* When user accomplishes the objective, the next level is loaded.
 
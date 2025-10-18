Connect-4 with Negamax AI

Using our board game engine, Connect 4 was created.

- First I looked at the prexisting examples such as othello and checkers to see how a more complex game is handled.
- I then started out by making placing down pieces work by placing it at the lowest part of the column selected.
- For the win conditions I just used an array of directions to check in and if at anypoint before the count is 4 it is not the same player I break the loop
- For checking draws I just checked if the board is completely full
- The AI was definetly the real challenge of the assignment as I had to look at some more videos of the negamax algorithm to really nail it down and had to create additional helper functions.
- I then added additional buttons for starting the connect4 with just two people or vs ai starting first or second.

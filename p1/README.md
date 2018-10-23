# BADMINTON ACADEMY PROBLEM

#### Running the code
`gcc 1.c -pthread`
`./a.out`
Then enter the number of refrees.

#### Structs
1) Person
Holds the state of the person, ie whether he has entered court, or meeting the organiser, or warming up etc

2) Organizer
Holds information of the organizer like the number of players the organiser has met, number of refrees he has met, whether he is allowing anyone to enter the court and how many people he has allowed to enter the court.

3) Args
Used for passing information about a person into the various functions like the person's id and whether he is a player or refree.

#### Threads
There is a person thread which is used for both players and refrees.

#### Functions 
1) void *enterAcademy(void * args)
The threads call this function wen thy are created. It is used by the players and refrees to enter the academy.

2) void meetOrganizer(Args *args)
Player/Refree has entered the court and he now meets the organizer.

3) void enterCourt(Args *arg)
Player/Refree has met the organizer. This function waits for a team to be formed and then the team of 2 players and a refree enter the court.

4) void warmUp(Args *arg)
Once the player has entered the court, he warms up.

5) void adjustEquipment(Args *arg)
After entering the court, refree starts adjusting the equipment.

6) void startGame(Args *arg)
Once the refree has finished adjusting the equipment and players have warmed up, the refree starts the game. Once the game has started, the organizer is free to meet other players and refrees.

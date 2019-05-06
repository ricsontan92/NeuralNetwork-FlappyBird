# NeuralNetwork-FlappyBird
Playing Flappy bird using Neural Network and Genetic Algorithm in C++

**************************** Neural network ****************************
- Takes in 3 inputs
  1) Horizatontal distance from the bird to the nearest obstacle
  2) Vertical distance from the bird to the opening in the nearest obstacle
  3) Vertical distance from the bird to the opening in the SECOND nearest obstacle
- Has no Hidden layers

**************************** Genetic Algorithm ****************************
Let N be the number of agent spawn per generation

- Selection
  + User can select number of agents to be spawned per generation (default: 50)
  + Agent's fitness is determined by (in order):
    1) How long it survived
    2) Vertical distance from the bird to the opening in the nearest obstacle
    For example,
      Bird 1: 100 points, 200 Vertical Distance
      Bird 2: 50 points, 50 Vertical Distance
      Bird 3: 50 points, 10 Vertical Distance
      
      1st Place(Fittesst) : Bird 1 (Because it survived the longest)
      2nd Place           : Bird 3 (Because the points of 2 and 3 are the same, vertical distance will be used)
      3rd Place           : Bird 2
 
 + Only 10% of the agents will be selected (E.g. out of 50, only 5 is selected for mating)
  
- Crossover
  + Generate N agents using (N * 0.1) parents
  + Parents will be matched randomly with it's mating pair
  + Genes are the weights of the neural network
    Since there are NO hidden layers, 3 inputs, there will be only 4 genes(weights) per parent:
      Weights of 3 inputs + 1 weight of bias
  + 2 of the genes(50%) will be selected to crossover or be mutated
  + Mutate policy
    Select a random number between -1.0 to 1.0

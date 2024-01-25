import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from tqdm import tqdm
from tqdm import trange
import json 

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(device)
class TicTacToe():
    def __init__(self, player1, player2):
        self.players = [player1, player2]

        self._reward = {0: 0, 1: 1, 2: -1}

    def play(self, num_games = 1, visualize=False):
        transitions = []
        for _ in range(0, num_games):
            turn = 1
            state = np.zeros((3,3), dtype=np.int64) # GET BOARD
            
            # state = (state2d, turn) # *
            for i in range(9):
                current_player = self.players[turn-1]
                action = current_player.get_action(state)# *
                print(action)
                next_state, finish, reward = self.play_turn(state, action) # *
                transitions.append((state, action, next_state, reward)) # *
                if (visualize):
                    self.visualize(next_state) # * 
                state = next_state # *
                
                
                if (finish):
                    break

        return transitions
    
    def getPlayerMove(self):
        col = int(input("What column would you like to play in? "))
        row = int(input("What row would you like to play in?" ))
        return torch.tensor(row*3 + col, dtype=torch.int64)


    def play_turn(self, state, action): # *
        # state2d, turn = state # * 
        next_state = state.copy() # *
        # next_turn = (turn)%2+1

        ax, ay = action // 3, action%3
        if (state[ax, ay]) != 0: # *
            # print("invalid state")
            next_state.fill(0) # * # USE DIFFERENT VALUE
            return next_state, self._reward[0] # * # MAYBE CHANGE TO LOSS
        
        next_state[ax, ay] = action # FIX

        # win
        if (self.winner(next_state, ax, ay)): # *
            # print("win")
            next_state = (next_state, 0) # *
            return next_state, self._reward[1] # *

        # tie
        if (next_state != 0).all(): # *
            # print("tie")
            return next_state, self._reward[0] # *
        
        # continue
        return next_state, self._reward[0] # *

    def winner(self, board, x, y):
        type = board[x, y]
        # print(type, " ", x, " ", y)
        ld = True
        rd = True
        r = True
        c = True
        # print(board.shape[0])
        for i in range(0, board.shape[0]):
            
            # for j in range(0, board.shape[1]):
            if (board[x, i] != type):
                r = False
            if (board[i, y] != type):
                c = False

            if ((x != y) or board[i, i] != type):
                ld = False
            if ((x != 2-y) or board[i, 2-i] != type):
                rd = False
        # print (ld, " ", rd, " ", r, " ", c, " ")
        return (ld or rd or r or c)

class QModel(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.embedding = torch.nn.Embedding(3,3)
        self.layer1 = torch.nn.Linear(30, 300)
        self.layer2 = torch.nn.Linear(300, 300)
        self.layer3 = torch.nn.Linear(300, 9)
        self.relu = torch.nn.ReLU()
    
    def forward(self): # *
        if not torch.is_tensor(states): # *
            states = torch.from_numpy(states) # *
        # if not torch.is_tensor(turns):
        #     states2d = torch.from_numpy(turns) # *
        assert states.dim() == 9 # * # CHECK
        # assert turns.dim() == 1

        x = torch.cat([states.flatten(1)], 1) # * # FIGURE OUT WHAT 1 MEANS
        x = self.relu(self.embedding(x)).flatten(1)
        #           /
        #          /
        #         /
        #________/
        x = self.relu(self.layer1(x))
        x = self.relu(self.layer2(x))
        x = self.layer3(x)
        return x
    
    def _serialize_tensor(self, tensor):
        if (tensor.dim() == 0):
            return float(tensor)
        return [self._serialize_tensor(x) for x in tensor]
    
    def _deserialize_tensor(self, tensor):
        return torch.tensor(tensor, dtype=torch.get_default_dtype())
    
    def save (self, filename):
        if (not filename.endswith(".json")):
            filename += ".json"
        with open(filename, "w") as file:
            json.dump({k: self._serialize_tensor(t) for k, t in self.state_dict().items()}, file)

    def load(self, filename):
        if (not filename.endswith(".json")):
            filename += ".json"
        with open(filename, "r") as file:
            self.load_state_dict({k: self._deserialize_tensor(t) for k, t in json.load(file).items()})
        return self

class Agent():
    def __init__(self, qmodel = None, epsilon = 0.2, learning_rate = 0.01, discount_factor = 0.9):
        if (qmodel == None):
            self.qmodel = QModel()
        
        # chance of doing random action teehee
        self.epsilon = epsilon

        # how fast values are updated
        self.learning_rate = learning_rate

        self._optimizer = torch.optim.Adam(self.qmodel.parameters(), lr=learning_rate)

        self.discount_factor = discount_factor

    def random_action(self):
        return int(np.random.randint(0, 9, 1, dtype=np.int64))
    
    def best_action(self, state): # *
        with torch.no_grad():
            # state2d, turn = state # *
            # sign = np.float64(1-2*(turn-1))
            # turns = torch.tensor(turn, dtype=torch.int64)[None] # da hell? "bach dim requjired"
            state = torch.tensor(state, dtype=torch.int64)[None] # *
            qvalues = self.qmodel(state)[0] # *
        return np.argmax(qvalues)
    
    def get_action(self, state): # *
        if np.random.rand() < self.epsilon:
            return self.random_action()
        else:
            return self.best_action(state) # *
        
    def learn(self, transitions):
        states, actions, next_states, rewards = zip(*transitions) # *
        states = zip(*states)  # *
        next_states = zip(*next_states) # *

        states = torch.tensor(np.array(states), dtype=torch.int64) # *
        next_states = torch.tensor(np.array(next_states), dtype=torch.int64) # *
        actions = torch.tensor(actions, dtype=torch.int64)
        rewards = torch.tensor(rewards, dtype=torch.float32)

        with torch.no_grad():
            # mask = (next_turns>0).float() # FIGURE THIS OUT
            next_qvalues = self.qmodel(next_states) # *
            expected_qvalues_for_actions = rewards + (self.discount_factor * torch.max(next_qvalues, 1)[0]) # mask * 

        q_values_for_actions = torch.gather(self.qmodel(states), dim=1, index=actions[:, None]).view(-1) # *
        loss = torch.nn.functional.smooth_l1_loss(q_values_for_actions, expected_qvalues_for_actions)
        self._optimizer.zero_grad()
        loss.backward()
        self._optimizer.step()
        return loss.item()





def trainNewModel():
    np.random.seed(3)
    torch.manual_seed(1)
    total_number_of_games = 15_000_00
    number_of_games_per_batch = 1000

    player = Agent(epsilon = 0.7, learning_rate = 0.01)
    game = TicTacToe(player, player)

    min_loss = np.inf
    range_ = trange(total_number_of_games//number_of_games_per_batch)

    for i in range_:
        transitions = game.play(num_games=number_of_games_per_batch)
        np.random.shuffle(transitions)
        loss = player.learn(transitions)

        if (loss < min_loss and loss > 0.01):
            min_loss = loss

        range_.set_postfix(loss=loss, min_loss=min_loss)

    player.qmodel.save("qmodel.json")

def playModel():
    player = Agent(epsilon = 0.0)
    game = TicTacToe(player, player)
    player.qmodel.load("qmodel.json")

    game.play(num_games=1, visualize=True)

def humanvBot():
    player = Agent(epsilon = 0.0)
    game = TicTacToe(None, player)
    player.qmodel.load("qmodel.json")

    game.playHuman()
    
# playModel()
# trainNewModel()
humanvBot()
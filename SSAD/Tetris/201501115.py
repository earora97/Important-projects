#!/usr/bin/env python2
#-*- coding: utf-8 -*-

from random import randrange as rand
import pygame, sys

# The configuration
cell_size =	10
cols =		32
rows =		30
maxfps = 	1
board = [[0 for x in range(cols)] for y in range(rows)] 

shapes = [ # Defining the blocks for the game
	[[1, 1, 1],
	 [0, 1, 0]],
	
	[[0, 1],
	 [1, 1]],
	
	[[1, 1, 0],
	 [0, 1, 1]],
	
	[[1],
	 [1],
	
	[[1, 1, 1],
	 [1, 1, 1]],
	
	[[1, 1, 1, 1]],
]

black = (0,0,0)
white = (255,255,255)
pygame.display.set_caption('My game')
clock = pygame.time.Clock()

pygame.font.init()
font = pygame.font.SysFont("Grobold",25)


class Board(object):
	def __init__(self):
		super(Board, self).__init__()
	

	def check_piece_pos(self,nx,ny,board,shape):
		for x,row in enumerate(shape):
			for y,cell in enumerate(row):
				if cell==1 and board[nx+x-1][ny+y-1]==1:
					return False
				return True

bb =  Board()


class Block(object):
		def __init__(self):
			super(Block, self).__init__() #for inheritance
		

		def moveleft(self,stone,pos1,pos2):
			x=pos1
			y=pos2
			if x<0:
				x=0
			elif bb.check_piece_pos(x-1,y,board,stone):
				tt.stonex -= 1
				tt.draw_board(board,0,0)
				tt.draw_board(stone,x-1,y)
			pygame.display.update()

		def moveright(self,stone,pos1,pos2):
			x=pos1
			y=pos2
			if x<0:
				x=0
			elif bb.check_piece_pos(x+1,y,board,stone):
				tt.stonex += 1
				tt.draw_board(board,0,0)
				tt.draw_board(stone,x+1,y)
			pygame.display.update()
			#draw_board(x-1,y) #niche jakar ruk jaye iske liye bhi likhna hai

		def fillPiecePos(self,tone,pos1,pos2):
			update_score(1)
			x=pos1
			y=pos2
			if(y==0):
				game_end()
			for x1,row in enumerate([stone]):
				for y1,cell in enumerate([row]):
					if cell == 1:
						board[x+x1-1][y+y1-1]=1
			check_row_full()
			tt.draw_board(board,0,0)
			select_piece()
			tt.draw_board(tt.stone,tt.stonex,tt.stoney)

		def drop(self,stone,pos1,pos2):
			x=pos1
			y=pos2
			if(bb.check_piece_pos(x,y-1,board,stone)):
				y -= 1
				self.stoney = y
				tt.draw_board(board,0,0)
				tt.draw_board(stone,x,y-1)
				return True
			else:
				Block.fillPiecePos(stone,x,y)
				return False
b=Block()


class GamePlay(Board, Block): #GamePlay is inherited from Board and Block
	
	game_on=True
	def initialise_board(self):
		for x in xrange(rows):
			for y in xrange(cols):
				board[x][y]=0
	
	def select_piece(self):
		self.stone = shapes[rand(len(shapes))]
		self.stonex = cols/2 + len(self.stone[0])/2
		self.stoney = 0

	def message_to_screen(self,msg,color):
		screen_text = font.render(msg, True, color)
		self.screen.blit(screen_text,[20,20])

	
	def __init__(self):
		super(GamePlay, self).__init__() # for inheritance
		pygame.init() #initialisation
		#screen setup
		self.width = (cols+10)* cell_size
		self.height = rows*cell_size
		self.temp = cols*cell_size
		self.score = 0
		self.screen = pygame.display.set_mode((self.width, self.height))
		self.background = pygame.Surface(self.screen.get_size())
		self.background = self.background.convert()
		self.background.fill((250,150,100))
		self.screen.blit(self.background,(0,0))
		self.initialise_board()
		self.select_piece()
		pygame.display.update()
		#self.message_to_screen("Welcome to Tetrus. Press A to start", black)

	def check_row_full(shape):
		for y,row in enumerate(shape):
			for x,cell in enumerate(row):
				if cell!=1:
					break
			if x == len(row):
				delete_row(x,shape)
				update_score(2)
		pygame.display.update()

	def delete_row(dele,shape):
		for y,row in enumerate(shape):
			for x,cell in enumerate(row):
				if y == dele:
					shape[x][row-y]=0
				else:
					shape[x][row-y]=shapr[x-1][row-y]
					now_cell = shape[x][row-y]
					if now_cell == 1:
						pygame.draw.rect(self.screen,3,pygame.Rect((x-1)*size_cell,(y-1)*size_cell,size_cell,size_cell),0)

	def draw_board(self,shape,pos1,pos2):
		offx = pos1
		offy = pos2
		for y,row in enumerate([shape]):
			for x,cell in enumerate([row]):
				if cell == '1':
					pygame.draw.rect(self.screen,3,pygame.Rect((x+offx)*size_cell,(y+offy)*size_cell,size_cell,size_cell),0)
					

	def game_end(self):
			game_on = False
			Game.screen.fill(white)
			print("Score:",Game.score)  #correct
			print("Game Over\n")
	def update_score(self,score_mode):
		if score_mode == 1:
			Game.score += 10
		elif score_mode == 2:
			Game.score += 100


	def make_move(self,move):
		if move == 'd':
			b.moveright(tt.stone,tt.stonex,tt.stoney)
		elif move == 'a':
			b.moveleft(tt.stone,tt.stonex,tt.stoney)
		elif move == 's':
			self.rotate(self.stone)
		elif move == 'SPACE':
			self.drop_instant()

	def rotate(self,shape):
		return [ [ shape[y][x]
				for y in xrange(len(shape)) ]
			for x in xrange(len(shape[0]) - 1, -1, -1) ]

	def drop_instant(self):
		x = self.stonex
		y = self.stoney
		while 1:
			if drop(self.stone,x,y) == False:
				break

	def run(self):
		ttime = pygame.time.Clock()
		while self.game_on==True:
			
			self.draw_board(board,0,0)
			self.select_piece()
			self.draw_board(self.stone,self.stonex,self.stoney)
			b.drop(self.stone,self.stonex,self.stoney)
			#printgame.display.update()
			pygame.display.update()
			print "Enter next move"
			self.message_to_screen("Enter next move", black)
			pygame.display.update()
			self.message_to_screen(self.score, black)
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					self.game_on = False
			move = raw_input()

		
			self.make_move(move)
			
			print("Score:", self.score)
		
			clock.tick(maxfps)




if __name__ == '__main__':
	#Game = GamePlay()
	tt=GamePlay()
	tt.run()
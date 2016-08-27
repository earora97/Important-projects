import pygame,time,sys
from random import randrange as rand

pygame.init()

white = (255,255,255)
black = (0,0,0)
red = (255,0,0)
clock = pygame.time.Clock()
display_height = 600
display_width = 800

gameDisplay = pygame.display.set_mode((display_width,display_height))
pygame.display.set_caption('My game')


font = pygame.font.SysFont(None,25)

def message_to_screen(msg,color):
	screen_text = font.render(msg, True, color)
	gameDisplay.blit(screen_text,[display_width/2,display_height/2])

message_to_screen("Press any arrow key to start", black)

def gameLoop():
	gameExit = False
	gameOver = False
	lead_x = 0
	lead_y = 0
	lead_x_change=0
	lead_y_change=0
	block_sizey = 5
	block_sizex = 15
	display_height = 600
	display_width = 800
	FPS = 10
	while not gameExit:
		
		while gameOver == True:
			gameDisplay.fill(white)
			message_to_screen("Game over. C to again.. Q to quit",red)
			pygame.display.update()

			for event in pygame.event.get():
				if event.type == pygame.KEYDOWN:
					if event.key == pygame.K_q:
						gameExit = True
						gameOver = False
					if event.key == pygame.K_c:
						gameLoop()

		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				gameExit = True
			if event.type == pygame.KEYDOWN:
				if event.key == pygame.K_LEFT:
					lead_x -= block_sizex
				elif event.key == pygame.K_RIGHT:
					lead_x += block_sizex
				elif event.key == pygame.K_UP:
					lead_y_change = -block_sizey
				elif event.key == pygame.K_DOWN:
					lead_y_change = block_sizey
			if lead_x >= display_width or lead_x < 0 or lead_y >= display_height or lead_y <0:
				gameOver = True
		lead_y += lead_y_change
		gameDisplay.fill(white)
		pygame.draw.rect(gameDisplay,black,[lead_x,lead_y,20,20])
		pygame.display.update()

		clock.tick(FPS)



	pygame.quit()
	quit()

gameLoop()

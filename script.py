import pygame as pg
from random import choice, randrange

class Symbol:
    def __init__(self, x, y, speed):
        self.x, self.y = x, y
        self.speed = speed
        self.value = choice(green_binary)  # Initial random symbol
        self.change_interval = 300  # Static symbol change interval (in milliseconds)
        self.last_change_time = pg.time.get_ticks()  # Time of the last symbol change
        self.alpha = 0  # Initially the symbol is transparent
        self.is_transparent = True  # Flag indicating that the symbol is in a transparent state

    def update(self):
        # Current time
        current_time = pg.time.get_ticks()

        # Check if the static interval has passed
        if current_time - self.last_change_time >= self.change_interval:
            self.value = choice(green_binary)  # Change the symbol
            self.last_change_time = current_time  # Update the last change time

        # Change transparency
        if self.is_transparent:
            if self.alpha < randrange(100, 255):
                self.alpha += 5  # Gradually increase transparency
            else:
                self.is_transparent = False  # If we've reached the opaque state, toggle the flag
        else:
            if self.alpha > randrange(0, 10):
                self.alpha -= 5  # Gradually decrease transparency
            else:
                self.is_transparent = True  # If we've reached the transparent state, toggle the flag

        # Draw the symbol
        temp_surface = self.value.copy()  # Copy the surface with the symbol
        temp_surface.set_alpha(self.alpha)  # Set the level of transparency
        surface.blit(temp_surface, (self.x, self.y))

        # Move the symbol down
        self.y += self.speed
        if self.y > height:
            self.y = -font_size  # If the symbol goes out of bounds, return it to the top

class SymbolColumn:
    def __init__(self, x, y):
        self.column_height = randrange(8, 25)  # Random column height
        self.speed = randrange(3, 6)  # Random speed
        self.symbols = [Symbol(x, i, self.speed) for i in range(y, y - font_size * self.column_height, -font_size)]

        # Set half of the symbols in the column to be transparent
        for i in range(len(self.symbols) // 2):
            self.symbols[i].is_transparent = True

    def update(self):
        # Randomly choose which symbols become opaque
        if choice([True, False]):
            rand_index = randrange(len(self.symbols) // 2)
            self.symbols[rand_index].is_transparent = False

        # Update the symbols
        for symbol in self.symbols:
            symbol.update()

    def draw(self):
        for symbol in self.symbols:
            symbol.update()

# Initialize Pygame
pg.init()

# Set up the display to be full-screen
infoObject = pg.display.Info()
width, height = infoObject.current_w, infoObject.current_h
font_size = int(height / 30)  # Dynamic font size based on screen height

surface = pg.display.set_mode((width, height), pg.FULLSCREEN)
clock = pg.time.Clock()

# Generate symbols
binary = [int(i) for i in range(2) for j in range(100)]
font = pg.font.SysFont('Arial', font_size, bold=True)
green_binary = [font.render(str(i), True, pg.Color('green')) for i in binary]

# Create symbols
symbol_columns = [SymbolColumn(x, randrange(-height, 0)) for x in range(0, width, font_size)]

# Main loop
while True:
    surface.fill(pg.Color('black'))

    # Update and draw symbols
    for column in symbol_columns:
        column.update()
        column.draw()

    # Closing the program
    for event in pg.event.get():
        if event.type == pg.QUIT:
            pg.quit()
            exit()

    pg.display.flip()
    clock.tick(60)

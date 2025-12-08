import pygame
import random
import sys

# Simple Snake game using pygame

CELL = 20
COLS = 30
ROWS = 20
WIDTH = CELL * COLS
HEIGHT = CELL * ROWS
FPS = 30

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GREEN = (0, 200, 0)
RED = (200, 0, 0)
BLUE = (0, 120, 200)


def draw_grid(surface):
    for x in range(0, WIDTH, CELL):
        pygame.draw.line(surface, (40, 40, 40), (x, 0), (x, HEIGHT))
    for y in range(0, HEIGHT, CELL):
        pygame.draw.line(surface, (40, 40, 40), (0, y), (WIDTH, y))


class Snake:
    def __init__(self):
        self.positions = [(COLS // 2, ROWS // 2)]
        self.direction = (1, 0)
        self.grow_pending = 0

    def head(self):
        return self.positions[0]

    def move(self):
        x, y = self.head()
        dx, dy = self.direction
        new = ((x + dx) % COLS, (y + dy) % ROWS)
        if new in self.positions:
            return False
        self.positions.insert(0, new)
        if self.grow_pending > 0:
            self.grow_pending -= 1
        else:
            self.positions.pop()
        return True

    def change_direction(self, dir):
        # Prevent reversing
        dx, dy = dir
        cdx, cdy = self.direction
        if (dx, dy) == (-cdx, -cdy):
            return
        self.direction = (dx, dy)

    def grow(self):
        self.grow_pending += 1


class Food:
    def __init__(self, snake):
        self.position = (0, 0)
        self.randomize(snake)

    def randomize(self, snake):
        choices = [(x, y) for x in range(COLS) for y in range(ROWS) if (x, y) not in snake.positions]
        if choices:
            self.position = random.choice(choices)
        else:
            self.position = None


def draw(surface, snake, food, score):
    surface.fill(BLACK)
    draw_grid(surface)
    # draw food
    if food.position:
        fx, fy = food.position
        pygame.draw.rect(surface, RED, (fx * CELL, fy * CELL, CELL, CELL))
    # draw snake
    for i, (x, y) in enumerate(snake.positions):
        rect = (x * CELL, y * CELL, CELL, CELL)
        color = GREEN if i == 0 else BLUE
        pygame.draw.rect(surface, color, rect)
    # draw score
    font = pygame.font.SysFont(None, 24)
    txt = font.render(f"Score: {score}", True, WHITE)
    surface.blit(txt, (5, 5))
    pygame.display.flip()


def game_over_screen(surface, score):
    font = pygame.font.SysFont(None, 48)
    small = pygame.font.SysFont(None, 28)
    surface.fill(BLACK)
    go = font.render("Game Over", True, RED)
    sc = small.render(f"Score: {score}", True, WHITE)
    hint = small.render("Press R to restart or Q/Esc to quit", True, WHITE)
    surface.blit(go, (WIDTH // 2 - go.get_width() // 2, HEIGHT // 3))
    surface.blit(sc, (WIDTH // 2 - sc.get_width() // 2, HEIGHT // 2))
    surface.blit(hint, (WIDTH // 2 - hint.get_width() // 2, HEIGHT // 2 + 40))
    pygame.display.flip()


def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("贪吃蛇 - 简单实现")
    clock = pygame.time.Clock()

    snake = Snake()
    food = Food(snake)
    score = 0
    running = True
    dead = False

    while running:
        clock.tick(FPS)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_UP or event.key == pygame.K_w:
                    snake.change_direction((0, -1))
                elif event.key == pygame.K_DOWN or event.key == pygame.K_s:
                    snake.change_direction((0, 1))
                elif event.key == pygame.K_LEFT or event.key == pygame.K_a:
                    snake.change_direction((-1, 0))
                elif event.key == pygame.K_RIGHT or event.key == pygame.K_d:
                    snake.change_direction((1, 0))
                elif event.key == pygame.K_ESCAPE:
                    running = False
                elif dead and (event.key == pygame.K_r):
                    # restart
                    snake = Snake()
                    food = Food(snake)
                    score = 0
                    dead = False

        if not dead:
            ok = snake.move()
            if not ok:
                dead = True
            # eat food
            if snake.head() == food.position:
                score += 1
                snake.grow()
                food.randomize(snake)

        draw(screen, snake, food, score)

        if dead:
            game_over_screen(screen, score)
            # handle restart / quit in events above

    pygame.quit()
    sys.exit()


if __name__ == '__main__':
    main()

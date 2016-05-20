#! /usr/bin/python
#! encoding: utf-8

import sys, pygame
import time

from config import *

answers = [ "C", "B", "A", "D", "A" ]

def get_rect(origin):
    return pygame.Rect(origin, button_size)

rectA = get_rect(originA)
rectB = get_rect(originB)
rectC = get_rect(originC)
rectD = get_rect(originD)

def get_button(pos):
    if rectA.collidepoint(pos):
        return "A"
    if rectB.collidepoint(pos):
        return "B"
    if rectC.collidepoint(pos):
        return "C"
    if rectD.collidepoint(pos):
        return "D"
    return None


def check_exit(event):
    do_exit = (event.type == pygame.QUIT) or (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE)
    if do_exit:
        sys.exit()

####################################################################################################

class Game:
    def __init__(self):
        pygame.init()

        # , pygame.FULLSCREEN
        self.screen = pygame.display.set_mode(screen_size)

        self.entry = pygame.image.load("png/entry.png").convert()

        self.errors = {
            "rus" : pygame.image.load("png/errors-{}.png".format("rus")).convert(),
            "eng" : pygame.image.load("png/errors-{}.png".format("eng")).convert()
        }

        self.lang = "rus"
        self.questions = {
            "rus" : [],
            "eng" : []
        }

        self.digits = []
        for i in xrange(len(answers)):
            x = pygame.image.load("png/d{}.png".format(i)).convert()
            x.set_colorkey((0, 0, 0))
            self.digits.append(x)

        for i in xrange(len(answers)):
            self.questions["rus"].append(pygame.image.load("png/q{}-{}.png".format(i, "rus")).convert())
            self.questions["eng"].append(pygame.image.load("png/q{}-{}.png".format(i, "eng")).convert())

        again_rus_orig = 486, 719
        again_rus_size = 280, 120
        self.again = {}
        self.again["rus"] = pygame.Rect(again_rus_orig, again_rus_size)

        again_eng_orig = 530, 709
        again_eng_size = 207, 132
        self.again["eng"] = pygame.Rect(again_eng_orig, again_eng_size)

        self.hooray = pygame.image.load("png/hooray.jpg").convert()

    def select_language(self):
        self.screen.blit(self.entry, (0, 0))
        pygame.display.flip()

        eng_origin = 690, 360
        eng_size = 335, 200
        eng = pygame.Rect(eng_origin, eng_size) 

        rus_origin = 231, 360
        rus_size = 335, 200
        rus = pygame.Rect(rus_origin, rus_size)

        while True:
            for event in pygame.event.get():
                check_exit(event)

                if event.type == pygame.MOUSEBUTTONDOWN:
                    print event.pos
                    if rus.collidepoint(event.pos):
                        print "Russian"
                        return"rus"

                    if eng.collidepoint(event.pos):
                        print "English"
                        return "eng"


    def show_questions(self, lang):
        change_question = True
        question_index = 0
        error_count = 0
        while question_index < len(answers):
            if change_question:
                new_question = self.questions[lang][question_index]
                self.screen.blit(new_question, (0, 0))
                pygame.display.flip()
                change_question = False

            for event in pygame.event.get():
                check_exit(event)
                if event.type == pygame.MOUSEBUTTONDOWN:
                    button = get_button(event.pos)
                    print button, event.pos
                    if button:
                        if button != answers[question_index]:
                            error_count += 1
                        question_index += 1
                        change_question = True

        return error_count

    def get_result(self, error_count, lang):
        background = self.errors[lang].copy()
        print "Error count {}".format(error_count)

        digit_pos = 901, 365
        background.blit(self.digits[error_count - 1], digit_pos)
        return background

    def show_black(self):
        black = pygame.Surface(screen_size)
        self.screen.blit(black, (0, 0))
        pygame.display.flip()

        while True:
            for event in pygame.event.get():
                check_exit(event)

                if event.type == pygame.KEYDOWN:
                    return


    def show_result(self, error_count, lang):
        result = self.get_result(error_count, lang)
        self.screen.blit(result, (0, 0))
        pygame.display.flip()

        while True:
            for event in pygame.event.get():
                check_exit(event)

                if event.type == pygame.KEYDOWN:
                    return

                if event.type == pygame.MOUSEBUTTONDOWN:
                    print event.pos
                    if self.again[lang].collidepoint(event.pos):
                        return

    def show_success(self):
        self.screen.blit(self.hooray, (0, 0))
        pygame.display.flip()

        while True:
            for event in pygame.event.get():
                    check_exit(event)
                    if event.type == pygame.MOUSEBUTTONDOWN:
                        sys.exit()

    def run(self):
        while True:
            self.show_black()

            lang = self.select_language()
            while True:
                error_count = self.show_questions(lang)

                if error_count != 0:
                    self.show_result(error_count, lang)
                else:
                    break;

def main():
     g = Game()
     g.run()

if __name__ == "__main__":
    main()

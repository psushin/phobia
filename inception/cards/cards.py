#! /usr/bin/python
#! encoding: utf-8

import sys, pygame
import time
import random
import serial

from os import listdir
import os

from collections import namedtuple

####################################################################################################

def check_exit(event):
    'Defines pygame events that quit immediately'
    do_exit = (event.type == pygame.QUIT) or (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE)
    if do_exit:
        sys.exit()

def load_cards(path):
    pictures = []
    for file in listdir(path):
        filepath = os.path.join(path, file)
        pic = pygame.image.load(filepath).convert()
        pictures.append(pygame.transform.scale(pic, (256, 256)))
    return pictures

def get_card_coordinate(index):
    assert 0 <= index < 12
    x_origin = 70
    y_origin = 62
    delta = 295

    x = x_origin + delta * (index % 4)
    y = y_origin + delta * (index / 4)
    return (x, y)

def get_card_index(x, y):
    x_origin = 70
    y_origin = 62
    delta = 295
    size = 256

    xm = x - x_origin
    ym = y - y_origin

    x_index = xm / delta
    y_index = ym / delta

    index = y_index * 4 + x_index

    if index > 11:
        return None

    x_card, y_card = get_card_coordinate(index)

    if x > x_card + size:
        return None

    if y > y_card + size:
        return None

    return index

def get_mark_coordinate(index):
    x_origin = 803
    y_origin = 935
    delta = 100

    x = x_origin + index * delta
    return (x, y_origin)

####################################################################################################

# CRC for RS482 by Eugene.

crc_tab = bytearray([
    0,  94, 188, 226, 97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
    157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
    35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
    190, 224, 2,  92, 223, 129, 99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
    70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89, 7,
    219, 133, 103,  57, 186, 228, 6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
    248, 166,  68,  26, 153, 199,  37, 123, 58, 100, 134, 216,  91,   5, 231, 185,
    140, 210,  48, 110, 237, 179,  81,  15, 78,  16, 242, 172,  47, 113, 147, 205,
    17, 79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
    175, 241,  19,  77, 206, 144, 114, 44, 109, 51, 209, 143,  12,  82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76,  18, 145, 207,  45, 115,
    202, 148, 118,  40, 171, 245,  23, 73, 8, 86, 180, 234, 105,  55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
    233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232,  10,  84, 215, 137, 107, 53])

def crc8(data):
    crc = 0;
    for i in xrange(len(data)):
        byte = data[i]
        crc = crc_tab[data[i] ^ crc]

    return crc

####################################################################################################

screen_size = 1280, 1024

address = 25

Card = namedtuple('Card', ['picture', 'correct'])

####################################################################################################

class Game:
    def __init__(self):
        pygame.init()

        # , pygame.FULLSCREEN
        self.screen = pygame.display.set_mode(screen_size)

        self.correct_animals = load_cards("./correct/animals")
        self.wrong_animals = load_cards("./wrong/animals")
        self.correct_transport = load_cards("./correct/transport")
        self.wrong_transport = load_cards("./wrong/transport")

        self.active_mark = pygame.image.load("./active_mark.png").convert_alpha()
        self.passive_mark = pygame.image.load("./passive_mark.png").convert_alpha()

        self.background = pygame.image.load("./background.png").convert()

        self.coin = pygame.image.load("./coin.png").convert_alpha()

        self.serial = serial.Serial("/dev/ttyUSB0")

    def sample_cards(self):
        result = []
        for picture in random.sample(self.correct_animals, 2):
            result.append(Card(picture, True))

        for picture in random.sample(self.correct_transport, 2):
            result.append(Card(picture, True))

        for picture in random.sample(self.wrong_animals, 4):
            result.append(Card(picture, False))

        for picture in random.sample(self.wrong_transport, 4):
            result.append(Card(picture, False))

        return result

    def restart(self):
        self.cards = self.sample_cards()
        random.shuffle(self.cards)

        background = self.background.copy()

        for i, c in enumerate(self.cards):
            background.blit(c.picture, get_card_coordinate(i))

        for i in xrange(4):
            background.blit(self.passive_mark, get_mark_coordinate(i))

        self.screen.blit(background, (0, 0))
        pygame.display.flip()

        self.result = 0
        self.selected = set()
        self.complete_time = None
        self.completed = False


    def show_mark(self, mark_index):
        self.screen.blit(self.active_mark, get_mark_coordinate(mark_index))
        pygame.display.flip()

    def show_coin(self):
        self.screen.blit(self.coin, (0, 0))
        pygame.display.flip()

    def process_events(self):
        for event in pygame.event.get():
            check_exit(event)

            if self.complete_time:
                continue

            if event.type != pygame.MOUSEBUTTONDOWN:
                continue

            print event.pos
            index = get_card_index(event.pos[0], event.pos[1])

            if index is None:
                # outside active area
                continue

            if index in self.selected:
                # already selected
                continue

            if not self.cards[index].correct:
                # incorrect card
                self.restart()
                continue

            self.selected.add(index)
            self.show_mark(self.result)
            self.result += 1

            if self.result == 4:
                # Small timeout to show all 4 marks.
                self.complete_time = pygame.time.get_ticks()

    def read_message(self):
        print >>sys.stderr, "Start reading, waiting {0} bytes".format(self.serial.inWaiting())
        lst = []
        byte_count = ord(self.serial.read())
        lst.append(byte_count)
        for i in xrange(byte_count - 1  ):
            lst.append(ord(self.serial.read()))

        print >>sys.stderr, "Received message {0}".format(lst)

        crc = crc8(lst[:-1])
        if crc != lst[-1]:
            print >>sys.stderr, "Crc verification failed {0}".format(lst)

        dst_address = lst[1]
        if dst_address != address:
            return None

        return lst

    def write_message(self, msg):
        crc = crc8(msg)
        msg.append(crc)

        print >>sys.stderr, "Send message {0}".format(msg)

        serialized = ''.join([chr(x) for x in msg])
        self.serial.write(serialized)


    def process_network(self):
        while self.serial.inWaiting() > 0:
            msg = self.read_message()
            if not msg:
                continue

            dst_address, src_address, flags, command = msg[1:5]

            if command == 1:
                # Status
                status = 1 if self.completed else 0
                self.write_message([6, src_address, dst_address, flags, status])
            elif command == 2:
                # Presence
                self.write_message([6, src_address, dst_address, flags, command])
            elif command == 5:
                # Reset
                self.restart()
                self.write_message([6, src_address, dst_address, flags, 0])
        print >>sys.stderr, "No more data"

    def run(self):
        self.restart()

        while True:
            self.process_events()
            self.process_network()

            # give CPU some rest
            pygame.time.wait(5)

            if self.completed:
                continue

            if self.complete_time and pygame.time.get_ticks() - self.complete_time > 1000:
                self.completed = True
                self.show_coin()


def main():
     g = Game()
     g.run()

if __name__ == "__main__":
    main()

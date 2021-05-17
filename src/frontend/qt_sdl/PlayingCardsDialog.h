/*
    Copyright 2016-2021 Arisotura, Raphaël Zumer

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#ifndef PLAYINGCARDSDIALOG_H
#define PLAYINGCARDSDIALOG_H

#include <QDialog>
#include <QDir>
#include <QGroupBox>
#include <QLabel>

#include "types.h"

namespace Ui { class PlayingCardsDialog; }
class PlayingCardsDialog;

typedef struct
{
    QList<QString> Cards;
    bool Flipped;
    QLabel *TextLabel;
    QLabel *ImageLabel;
    QGroupBox *ControlsGroupBox;
} CardPile;

class PlayingCardsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayingCardsDialog(QWidget* parent);
    ~PlayingCardsDialog();

    static PlayingCardsDialog* currentDlg;
    static PlayingCardsDialog* openDlg(QWidget* parent)
    {
        if (currentDlg)
        {
            currentDlg->activateWindow();
            return currentDlg;
        }

        currentDlg = new PlayingCardsDialog(parent);
        currentDlg->show();
        return currentDlg;
    }
    static void closeDlg()
    {
        currentDlg = nullptr;
    }

    static bool supportsCart(char gameCode[5])
    {
        const char PLAYING_CARDS_GAMECODES[3][5] =
        {
            "AJQJ", // Magic Taizen (Japan)
            "AJQE", // Master of Illusion (USA)
            "AJQP", // Magic Made Fun - Perform Tricks That Will Amaze Your Friends! (Europe)
        };

        for (int i = 0; i < sizeof(PLAYING_CARDS_GAMECODES)/sizeof(PLAYING_CARDS_GAMECODES[0]); i++)
        {
            if (strcmp(gameCode, PLAYING_CARDS_GAMECODES[i]) == 0) return true;
        }

        return false;
    }

private slots:
    void reset(); // Resets the UI, removing all loaded cards
    bool processCardDirectory(QDir directory); // Parses a card image directory and loads the deck
    bool paintCard(CardPile *stack); // Redraws a card for a single stack, returning true on success and false on error
    void updateUI(); // Refreshes all UI elements
    void createStack(CardPile *stack, int index); // Generates new card stack UI elements at the specified grid index
    void deleteStack(CardPile *stack); // Deletes card stack UI elements

    void on_browse();

    void on_draw();
    void on_shuffle();
    void on_flip();

    void on_return();
    void on_rotate();

private:
    Ui::PlayingCardsDialog *ui;

    CardPile Deck;
    CardPile Hand;
    QList<CardPile> Stacks;
    const int MAX_STACKS = 15;
};

#endif // PLAYINGCARDSDIALOG_H


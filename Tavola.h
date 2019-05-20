/***************************************************************************
 *            Tavola.h
 *
 *  Mon Apr 16 18:37:59 2007
 *  Copyright  2007  Simone Tobia
 *  Email simone_tobia@tiscali.it
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 // v1.3

#ifndef TAVOLA
#define TAVOLA
#include <iostream>

class Tavola{
    bool fin;
    bool p1win;
    bool p2win;
    bool equal;

    const char PLAYER1_SYM;
    const char PLAYER2_SYM;
    const char FREE_SYM;
    
    friend std::istream& operator>>(std::istream&, Tavola &);//Chiede in input due interi che rappresentano la posizione dell'inserimento controllandone la correttezza.
    friend std::ostream& operator<<(std::ostream&, const Tavola &);
    
    char *tavola;
    unsigned int dimensione;
    unsigned int winsize;
    bool turno;
    unsigned int n_occupate;
    
    void copia(const Tavola &);
    bool check_winner(const char);
    bool check_winner_row(bool, const char)const;
    bool check_winner_diag(const char)const;
    bool check_winner_all_diag(const char)const;
    bool check_equality(const char sym)const;
    public:
        Tavola(unsigned int =3,unsigned int=3);
        Tavola(const Tavola &);
        Tavola& operator=(const Tavola &);
        ~Tavola(){delete[] tavola;};
        
        bool contrassegna(unsigned int, unsigned int, bool);
        bool cerchio(unsigned int, unsigned int)const;
        bool croce(unsigned int, unsigned int)const;
        bool libera(unsigned int, unsigned int)const;
        
        bool operator~(){return check_winner(PLAYER1_SYM);};
        bool operator!(){return check_winner(PLAYER2_SYM);};
        bool finished(){
            if(fin)return true;
            if(equality()||check_winner(PLAYER1_SYM)||check_winner(PLAYER2_SYM)||(n_occupate==dimensione*dimensione)){
                fin=true;
                return true;
            }else return false;
        };//Controlla se sulla tavola possono ancora essere effettuate delle mosse.
        bool equality(){
            if(equal)return true;
            if(check_equality(PLAYER1_SYM)&&check_equality(PLAYER2_SYM)){
                equal=true;
                return true;
            }else return false;
        };

        bool isWinnerOnRow();
        bool isWinnerOnColumn();
        int getWinnerRow(char sym, int &offset);
        int getWinnerColumn(char sym, int &offset);
        bool getWinnerDiag(char sym, int &offset, int &number);

        bool who_plays()const{return turno;};//Ritorna falso se tocca al giocatore 1;
        
        void cambia_turno(){turno=!turno;};//ATTENZIONE: Usare con cautela per non incorrere in errori di programmazione!!!
        
        unsigned int get_dimensione()const{return dimensione;}
        unsigned int get_winsize()const{return winsize;}
        unsigned int get_occupate()const{return n_occupate;}
};

#endif

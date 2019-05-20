/***************************************************************************
 *            Tavola.cpp
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
 
#include "Tavola.h"

void Tavola::copia(const Tavola &p){
    turno=p.turno;
    dimensione=p.dimensione;
    winsize=p.winsize;
    tavola=new char[dimensione*dimensione];
    for(unsigned int i=0;i<dimensione*dimensione;i++)tavola[i]=p.tavola[i];
    n_occupate=p.n_occupate;
    fin=p.fin;
    equal=p.equal;
    p1win=p.p1win;
    p2win=p.p2win;
}

Tavola::Tavola(unsigned int dimensione,unsigned int winsize):PLAYER1_SYM('O'), PLAYER2_SYM('X'), FREE_SYM('-'){
    this->turno=0;
    this->dimensione = dimensione;
    this->winsize = winsize;
    tavola=new char[dimensione*dimensione];
    for(unsigned int i=0;i<(dimensione*dimensione);i++)tavola[i]=FREE_SYM;
    n_occupate=0;
    this->fin=false;
    this->equal=false;
    this->p1win=false;
    this->p2win=false;
}

Tavola::Tavola(const Tavola &p):PLAYER1_SYM('O'), PLAYER2_SYM('X'), FREE_SYM('-'){
    copia(p);
}

Tavola& Tavola::operator=(const Tavola &p){
    if(this!=&p){//ANTIALIASING
        delete[] tavola;
        copia(p);
    }
    return *this;
}

bool Tavola::contrassegna(unsigned int r, unsigned int c, bool croce){
    if(r>dimensione-1||c>dimensione-1||((!croce) && (turno))||((croce) && (!turno)))return false;
    if(tavola[c*dimensione+r]=='-'){
        tavola[c*dimensione+r]=(turno?PLAYER2_SYM:PLAYER1_SYM);
        turno=!turno;
        n_occupate++;
        return true;
    }
    return false;
}

bool Tavola::cerchio(unsigned int r, unsigned int c)const{
    if(r<dimensione&&c<dimensione&&tavola[r*dimensione+c]==PLAYER1_SYM)return true;
    return false;
}

bool Tavola::croce(unsigned int r, unsigned int c)const{
    if(r<dimensione&&c<dimensione&&tavola[r*dimensione+c]==PLAYER2_SYM)return true;
    return false;
}

bool Tavola::libera(unsigned int r, unsigned int c)const{
    if(r<dimensione&&c<dimensione&&tavola[r*dimensione+c]==FREE_SYM)return true;
    return false;
}

bool Tavola::check_equality(const char sym)const{
    unsigned int founded=0,founded2=0;
    for(unsigned int i=0;i<dimensione;++i){
        for(unsigned int j=0;j<dimensione;++j){
            if(tavola[i*dimensione+j]==sym || tavola[i*dimensione+j]==FREE_SYM) founded++;
            else founded=0;
            if(tavola[j*dimensione+i]==sym || tavola[j*dimensione+i]==FREE_SYM) founded2++;
            else founded2=0;
            if(founded>=winsize || founded2>=winsize) return false;
        }
        founded=founded2=0;
    }

    founded=founded2=0;
    unsigned int founded3=0,founded4=0;
    for(unsigned int j=0;j<((dimensione-winsize)+1);++j){
        for(unsigned int i=0;i<dimensione;++i){
            if(tavola[(i+j)*dimensione+i]==sym || tavola[(i+j)*dimensione+i]==FREE_SYM) founded++;
            else founded=0;
            if(tavola[(i+j)*dimensione+(dimensione-1-i)]==sym || tavola[(i+j)*dimensione+(dimensione-1-i)]==FREE_SYM) founded2++;
            else founded2=0;
            if(tavola[(i)*dimensione+i+j]==sym || tavola[(i)*dimensione+i+j]==FREE_SYM) founded3++;
            else founded3=0;
            if(tavola[(i)*dimensione+(dimensione-1-i-j)]==sym || tavola[(i)*dimensione+(dimensione-1-i-j)]==FREE_SYM) founded4++;
            else founded4=0;
            if(founded>=winsize || founded2>=winsize || founded3>=winsize || founded4>=winsize) return false;
        }
        founded=founded2=founded3=founded4=0;
    }

    return true;
}

bool Tavola::check_winner_row(bool is_row, const char sym)const{
    unsigned int founded=0;
    for(unsigned int i=0;i<dimensione;i++){
        for(unsigned int j=0;j<dimensione;j++){
            if(tavola[(is_row?i:j)*dimensione+(is_row?j:i)]==sym){
                founded++;
            }else{
                founded=0;
            }
            if(founded==winsize){
                return true;
            }
        }
        founded=0;
    }
    return false;
}

bool Tavola::check_winner_diag(const char sym)const{    
    unsigned int i,founded=0;
    for(i=0;i<dimensione;++i){
        if(tavola[i*dimensione+i]==sym) founded++;
        else founded=0;
        if(founded==winsize)return true;
    }
    founded=0;
    for(i=0;i<dimensione;++i){
        if(tavola[i*dimensione+(dimensione-1-i)]==sym) founded++;
        else founded=0;
        if(founded==winsize)return true;
    }
    return false;
}

bool Tavola::check_winner_all_diag(const char sym)const{
    unsigned int i,founded=0,founded2=0;
    for(unsigned int j=0;j<((dimensione-winsize)+1);++j){
        for(i=0;i<dimensione-j;++i){
            if(tavola[(i+j)*dimensione+i]==sym) founded++;
            else founded=0;
            if(tavola[i*dimensione+i+j]==sym) founded2++;
            else founded2=0;
            if(founded>=winsize || founded2>=winsize)return true;
        }
        founded=founded2=0;
    }
    founded=0;
    founded2=0;
    for(unsigned int j=0;j<((dimensione-winsize)+1);++j){
        for(i=0;i<dimensione-j;++i){
            if(tavola[(i+j)*dimensione+(dimensione-1-i)]==sym) founded++;
            else founded=0;
             if(tavola[(i)*dimensione+(dimensione-1-i-j)]==sym) founded2++;
            else founded2=0;
            if(founded>=winsize || founded2>=winsize)return true;
        }
        founded=founded2=0;
    }
    return false;
}

bool Tavola::check_winner(const char sym){
    if(sym==PLAYER1_SYM && p1win)return true;
    else if(sym==PLAYER2_SYM && p2win)return true;
    if(!check_winner_row(true, sym))
        if(!check_winner_row(false, sym))
            if(!check_winner_all_diag(sym))
                return false;
    if(sym==PLAYER1_SYM) p1win=true;
    else p2win=true;
    return true;
}

std::ostream& operator<<(std::ostream &os, const Tavola &p){
    for(unsigned int i=0;i<p.dimensione;i++){
        for(unsigned int j=0;j<p.dimensione;j++)
            os << p.tavola[i*p.dimensione+j] << ' ';
        os << std::endl;
    }
    return os;
}

std::istream& operator>>(std::istream &is, Tavola &p){
    bool successo=false;
    unsigned int x, y;
    is>>x>>y;
    successo=p.contrassegna(x-1, y-1, p.turno);
    while(!successo){
        std::cout << "Errore, impossibile inserire nella posizione indicata." << std::endl << "Reinserisci la posizione(X Y): ";
        is>>x>>y;
        successo=p.contrassegna(x-1, y-1, p.turno);
    }
    return is;
}

bool Tavola::isWinnerOnColumn(){
    return check_winner_row(false,PLAYER1_SYM)||check_winner_row(false,PLAYER2_SYM);
}

bool Tavola::isWinnerOnRow(){
    return check_winner_row(true,PLAYER1_SYM)||check_winner_row(true,PLAYER2_SYM);
}

int Tavola::getWinnerColumn(char sym, int &offset){
    unsigned int founded=0;
    for(unsigned int i=0;i<dimensione;i++){
        for(unsigned int j=0;j<dimensione;j++){
            if(tavola[j*dimensione+i]==sym){
                founded++;
            }else founded=0;
            if(founded==winsize){
                offset=j-winsize;
                return i;
            }
        }
        founded=0;
    }
    return -1;
}

int Tavola::getWinnerRow(char sym, int &offset){
    unsigned int founded=0;
    for(unsigned int i=0;i<dimensione;i++){
        for(unsigned int j=0;j<dimensione;j++){
            if(tavola[i*dimensione+j]==sym){
                founded++;
            }else founded=0;
            if(founded==winsize){
                offset=j-winsize;
                return i;
            }
        }
        founded=0;
    }
    return -1;
}

bool Tavola::getWinnerDiag(char sym, int & offset, int &number){
    unsigned i,founded=0;
    for(number=0;number<((dimensione-winsize)+1);++number){
        for(i=0;i<dimensione;++i){
            if(tavola[(i+number)*dimensione+i]==sym) founded++;
            else founded=0;
            if(founded>=winsize){
                offset=i-winsize;
                number=-number;
                return true;
            }
        }
        founded=0;
    }
    founded=0;
    for(number=0;number<((dimensione-winsize)+1);++number){
        for(i=0;i<dimensione;++i){
            if(tavola[(i)*dimensione+i+number]==sym) founded++;
            else founded=0;
            if(founded>=winsize){
                offset=i-winsize;

                return true;
            }
        }
        founded=0;
    }
    founded=0;
    for(number=0;number<(dimensione-winsize+1);++number){
        for(i=0;i<dimensione;++i){
            if(tavola[(i+number)*dimensione+dimensione-i-1]==sym) founded++;
            else founded=0;
            if(founded>=winsize){
                offset=dimensione-1-i;
                number=-number;
                return false;
            }
        }
        founded=0;
    }
    founded=0;
    for(number=0;number<(dimensione-winsize+1);++number){
        for(i=0;i<dimensione;++i){
            if(tavola[(i)*dimensione+dimensione-i-1-number]==sym) founded++;
            else founded=0;
            if(founded>=winsize){
                offset=dimensione-1-i;
                return false;
            }
        }
        founded=0;
    }

    return false;
}

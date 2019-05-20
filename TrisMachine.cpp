/***************************************************************************
 *            TrisMachine.cpp
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
 
#include "TrisMachine.h"
#include <stdlib.h>
#include <QtGlobal>
using namespace std;
int TrisMachine::find_first_free(const Tavola &p, int &row){
    int *cols = new int[p.get_dimensione()*p.get_dimensione()];
    int *rows = new int[p.get_dimensione()*p.get_dimensione()];
    int last=0;
    for(row=p.get_dimensione()-1;row>=0;row--)
        for(int col=p.get_dimensione()-1;col>=0;col--)
            if(p.libera(row, col)){
                cols[last]=col;
                rows[last++]=row;
            }


    int extracted=(qrand()/(float)RAND_MAX)*(float)(last-1);
    row=rows[extracted];
    extracted=cols[extracted];
    delete rows;
    delete cols;
    if(!last) return -1;
    return extracted;
}

int TrisMachine::check_all_attacks(const Tavola &p, int &row, const bool croce, ia_level ia){
    int col=-1;
    for(int proximity=p.get_winsize();proximity>0&&col==-1;proximity--)
        if((col=check_attacks(p, row, p.get_dimensione()==3?!croce:croce, proximity, ia))==-1)
            col=check_attacks(p, row, p.get_dimensione()==3?croce:!croce, proximity, ia);
    return col;
}

int TrisMachine::check_attacks(const Tavola &p, int &row, const bool croce, const int &proximity, ia_level ia){
    row=0;
    int indice=-1;
    if(ia==HARDCORE && (indice=find_cross_solutions(p, croce, row, proximity))!=-1) return indice;
    if((indice=check_all_diag_attacks(p, row, croce, proximity, ia))==-1)
        if((indice=check_row_attacks(p, row, croce, proximity, ia))==-1)
            indice=check_col_attacks(p, row, croce, proximity, ia);
    return indice;
}

int TrisMachine::check_row_attacks(const Tavola &p, int &row, const bool croce, const int & proximity, ia_level ia){
    for(unsigned int indice=0;indice<p.get_dimensione();indice++)
        if(((ia!=HARDCORE && ia!=SVEGLIA) || check_col_possibility(p,indice,croce,false)) && count_sym_row(p, croce, indice)>=proximity){//&&!count_sym_row(p, !croce, indice)){
            row=indice;
            return find_row_defense(p,indice,croce,proximity);
        }
    return -1;
}

int TrisMachine::find_row_defense(const Tavola &p, int row, const bool croce, const int & proximity){
    int founded=0,target=-1;
    for(unsigned int i=0;(i<p.get_dimensione())&&(founded<proximity||target==-1);++i){
        if((croce?p.croce(row,i):p.cerchio(row,i)) || (founded && p.libera(row,i)))founded++;
        else founded=0;
        if(p.libera(row,i))target=i;
        if(i<p.get_dimensione()-1 && p.libera(row,i+1))target=i+1;
    }

    return founded==proximity&&target!=-1&&(p.libera(row,target))?target:-1;
}

bool TrisMachine::check_col_possibility(const Tavola &p, int col, const bool croce, const bool iscol){
    unsigned int founded=0;
    for(unsigned int i=0;i<p.get_dimensione();++i){
        if(iscol){
            if((croce?p.croce(i,col):p.cerchio(i,col)) || p.libera(i,col)) founded++;
            else founded=0;
        }else{
            if((croce?p.croce(col,i):p.cerchio(col,i)) || p.libera(col,i)) founded++;
            else founded=0;
        }
        if(founded>=p.get_winsize()) return true;
    }

    return false;
}

int TrisMachine::check_col_attacks(const Tavola &p, int &row, const bool croce, const int &proximity, ia_level ia){
    for(unsigned int indice=0;indice<p.get_dimensione();indice++)
        if(((ia!=HARDCORE && ia!=SVEGLIA) || check_col_possibility(p,indice,croce,true)) && count_sym_col(p, croce, indice)==proximity){//&&!count_sym_col(p, !croce, indice)){

            row = find_col_defense(p,indice,croce,proximity);
            return (row!=-1&&p.libera(row,indice))?indice:-1;
        }
    return -1;
}

int TrisMachine::find_col_defense(const Tavola &p, int col, const bool croce, const int & proximity){
    int founded=0,target=-1;
    for(unsigned int i=0;i<p.get_dimensione()&&(founded<proximity||target==-1);++i){
        if((croce?p.croce(i,col):p.cerchio(i,col)) || (founded && p.libera(i,col)))founded++;
        else founded=0;
        if(p.libera(i,col))target=i;
        if(i<p.get_dimensione()-1 && p.libera(i+1,col))target=i+1;
    }

    return target;
}

int TrisMachine::check_diag_attacks(const Tavola &p, int &row, const bool croce, const int &proximity, ia_level ia){
    for(unsigned int indice=1;indice<=2;indice++)
        if((ia!=HARDCORE || check_diag_possibility(p,indice==1,croce)) && count_sym_diag(p, croce, indice)==proximity){
            int aux = find_diag_defense(p,indice==1,croce,proximity);
            if(p.libera(aux,(indice==1?aux:p.get_dimensione()-aux-1))){
                row=aux;
                return indice==1?aux:p.get_dimensione()-aux-1;
            }
        }
    return -1;
}

bool TrisMachine::check_diag_possibility(const Tavola &p, bool first, const bool croce){
    unsigned int founded=0;
    for(unsigned int i=0;i<p.get_dimensione();++i){
        if(first){
            if((croce?p.croce(i,i):p.cerchio(i,i)) || p.libera(i,i)) founded++;
            else founded=0;
        }else{
            if((croce?p.croce(i,p.get_dimensione()-i-1):p.cerchio(i,p.get_dimensione()-i-1)) || p.libera(i,p.get_dimensione()-i-1)) founded++;
            else founded=0;
        }
        if(founded>=p.get_winsize()) return true;
    }

    return false;
}

int TrisMachine::find_diag_defense(const Tavola &p, bool first, const bool croce, const int & proximity){
    int founded=0,target=-1;
    for(unsigned int i=0;i<p.get_dimensione()&&(founded<proximity||target==-1);++i){
        if(first){
            if((croce?p.croce(i,i):p.cerchio(i,i)) || (founded && p.libera(i,i)))founded++;
            else founded=0;
            if(p.libera(i,i))target=i;
            if(i<p.get_dimensione()-1 && p.libera(i+1,i+1))target=i+1;
        }else{
            if((croce?p.croce(i,p.get_dimensione()-i-1):p.cerchio(i,p.get_dimensione()-i-1)) || (founded && p.libera(i,p.get_dimensione()-i-1)))founded++;
            else founded=0;
            if(p.libera(i,p.get_dimensione()-i-1))target=i;
            if(i<p.get_dimensione()-1 && p.libera(i+1,p.get_dimensione()-i-2))target=i+1;
        }
    }

    return target;
}

int TrisMachine::check_all_diag_attacks(const Tavola &p, int &row, const bool croce, const int &proximity, ia_level ia){
    for(unsigned int j=0;j<(2*(p.get_dimensione()-p.get_winsize())+1);++j){
        if(((ia!=HARDCORE && ia!=SVEGLIA) || check_all_diag_possibility(p,true,croce,j)) && count_sym_all_diag(p, croce, j, true)==proximity){
            unsigned int diagonale=j;
            int aux = find_all_diag_defense(p,true,croce,proximity,j);

            if(aux==-1) continue;

            bool upper=diagonale>p.get_dimensione()-p.get_winsize();

            if(upper) diagonale-=p.get_dimensione()-p.get_winsize();            

            if(upper){
                if(p.libera(aux+diagonale,aux)){
                    row=aux+diagonale;
                    return aux;
                }
            }else{
                if(p.libera(aux,aux+diagonale)){
                    row=aux;
                    return aux+diagonale;
                }
            }
        }
    }        

    for(unsigned int j=0;j<(2*(p.get_dimensione()-p.get_winsize())+1);++j){
        if(((ia!=HARDCORE && ia!=SVEGLIA) || check_all_diag_possibility(p,false,croce,j)) && count_sym_all_diag(p, croce, j, false)==proximity){
            unsigned int diagonale=j;
            int aux = find_all_diag_defense(p,false,croce,proximity,j);


            if(aux==-1) continue;

            bool upper=diagonale>p.get_dimensione()-p.get_winsize();

            if(upper) diagonale-=p.get_dimensione()-p.get_winsize();            

            if(upper){
                if(p.libera(aux+diagonale,p.get_dimensione()-1-aux)){
                    row=aux+diagonale;
                    return p.get_dimensione()-1-aux;
                }
            }else{
                if(p.libera(aux,p.get_dimensione()-1-aux-diagonale)){
                    row=aux;
                    return p.get_dimensione()-1-aux-diagonale;
                }
            }
        }
    }

    return -1;
}

int TrisMachine::count_sym_all_diag(const Tavola &p, const bool croce, const unsigned int &diag, const bool first){
    unsigned int founded=0;
    unsigned int diagonale=diag;
    bool upper=diagonale>p.get_dimensione()-p.get_winsize();


    if(upper) diagonale-=p.get_dimensione()-p.get_winsize();

    for(unsigned int i=0;i<p.get_dimensione()-diagonale;++i){
        if(upper){
            if(croce?p.croce(i+diagonale,first?i:p.get_dimensione()-1-i):p.cerchio(i+diagonale,first?i:p.get_dimensione()-1-i))
                founded++;
        }else{
            if(croce?p.croce(i,first?i+diagonale:p.get_dimensione()-1-i-diagonale):p.cerchio(i,first?i+diagonale:p.get_dimensione()-1-i-diagonale))
                founded++;
        }
    }

    return founded;
}

bool TrisMachine::check_all_diag_possibility(const Tavola &p, bool first, const bool croce, const unsigned int &diag){
    unsigned int founded=0;
    unsigned int diagonale=diag;
    bool upper=diagonale>p.get_dimensione()-p.get_winsize();

    if(upper) diagonale-=p.get_dimensione()-p.get_winsize();


    for(unsigned int i=0;i<p.get_dimensione()-diagonale;++i){
        if(upper){
            if(first){
                if((croce?p.croce(i+diagonale,i):p.cerchio(i+diagonale,i)) || p.libera(i+diagonale,i)) founded++;
                else founded=0;
            }else{
                if((croce?p.croce(i+diagonale,p.get_dimensione()-i-1):p.cerchio(i+diagonale,p.get_dimensione()-i-1)) || p.libera(i+diagonale,p.get_dimensione()-i-1)) founded++;
                else founded=0;
            }
        }else{
            if(first){
                if((croce?p.croce(i,i+diagonale):p.cerchio(i,i+diagonale)) || p.libera(i,i+diagonale)) founded++;
                else founded=0;
            }else{
                if((croce?p.croce(i,p.get_dimensione()-1-i-diagonale):p.cerchio(i,p.get_dimensione()-1-i-diagonale)) || p.libera(i,p.get_dimensione()-1-i-diagonale)) founded++;
                else founded=0;
            }
        }
        if(founded>=p.get_winsize()) return true;
    }

    return false;
}

int TrisMachine::find_all_diag_defense(const Tavola &p, bool first, const bool croce, const int & proximity, const unsigned int &diag){
    int founded=0,target=-1;
    unsigned int diagonale=diag;
    bool upper=diagonale>p.get_dimensione()-p.get_winsize();

    if(upper) diagonale-=p.get_dimensione()-p.get_winsize();

    for(unsigned int i=0;(i<(p.get_dimensione()-diagonale))&&(founded<proximity||target==-1);++i){
        if(upper){
            if(first){
                if((croce?p.croce(i+diagonale,i):p.cerchio(i+diagonale,i)) || (founded && p.libera(i+diagonale,i)))founded++;
                else founded=0;
                if(p.libera(i+diagonale,i))target=i;
                if(i<p.get_dimensione()-diagonale-1 && p.libera(i+1+diagonale,i+1))target=i+1;
            }else{
                if((croce?p.croce(i+diagonale,p.get_dimensione()-i-1):p.cerchio(i+diagonale,p.get_dimensione()-i-1)) || (founded && p.libera(i+diagonale,p.get_dimensione()-i-1)))founded++;
                else founded=0;
                if(p.libera(i+diagonale,p.get_dimensione()-i-1))target=i;
                if(i<p.get_dimensione()-1-diagonale && p.libera(i+1+diagonale,p.get_dimensione()-i-2))target=i+1;
            }
        }else{
            if(first){
                if((croce?p.croce(i,i+diagonale):p.cerchio(i,i+diagonale)) || (founded && p.libera(i,i+diagonale)))founded++;
                else founded=0;
                if(p.libera(i,i+diagonale))target=i;
                if(i<p.get_dimensione()-diagonale-1 && p.libera(i+1,i+1+diagonale))target=i+1;
            }else{
                if((croce?p.croce(i,p.get_dimensione()-1-i-diagonale):p.cerchio(i,p.get_dimensione()-1-i-diagonale)) || (founded && p.libera(i,p.get_dimensione()-1-i-diagonale)))founded++;
                else founded=0;
                if(p.libera(i,p.get_dimensione()-1-i-diagonale))target=i;
                if(i<p.get_dimensione()-diagonale-1 && p.libera(i+1,p.get_dimensione()-2-i-diagonale))target=i+1;
            }
        }
    }
    return target;
}

int TrisMachine::find_cross_solutions(const Tavola &p, const bool &croce, int &riga, const int &proximity){
    int founded=0,tcol=-1;
    for(int i=0;i<p.get_dimensione();++i){
        if(check_col_possibility(p,i,croce,false)){
            for(int j=0;j<p.get_dimensione();++j){
                if(p.libera(i,j) && check_col_possibility(p,j,croce,true) && count_sequence(p,croce,i,j,true)>=proximity){
                    int actual=0;
                    bool empty=true;
                    for(int k=i+1;k<p.get_dimensione();++k){
                        if(croce?p.croce(k,j):p.cerchio(k,j)){
                            empty=false;
                            actual+=3;
                        }
                        else if(p.libera(k,j) && empty) actual+=2;
                        else break;
                    }
                    bool empty2=true;
                    for(int k=i-1;k>=0;k--){
                        if(croce?p.croce(k,j):p.cerchio(k,j)){
                            empty2=false;
                            actual+=3;
                        }
                        else if(p.libera(k,j) && empty2) actual+=2;
                        else break;
                    }
                    if((!empty || !empty2) && founded<actual){
                        founded=actual;
                        riga=i;
                        tcol=j;
                    }
                }
            }
        }
    }

    return tcol;
}

int TrisMachine::count_sequence(const Tavola &p, bool croce, unsigned int row, unsigned int col, bool iscol){
    int count=0;
    for(int i=iscol?row:col;i<p.get_dimensione();++i){
        if(croce?p.croce(iscol?i:row,iscol?col:i):p.cerchio(iscol?i:row,iscol?col:i))count++;
        else return count;
    }
    return count;
}

int TrisMachine::count_sym_row(const Tavola &p, const bool croce, const unsigned int &riga){
    unsigned int trovati=0;
    for(unsigned int colonna=0;colonna<p.get_dimensione();colonna++)
        if((croce?p.croce(riga, colonna):p.cerchio(riga, colonna)))trovati++;
    return trovati;
}

int TrisMachine::count_sym_col(const Tavola &p, const bool croce, const unsigned int &colonna){
    unsigned int trovati=0;
    for(unsigned int riga=0;riga<p.get_dimensione();riga++)
        if((croce?p.croce(riga, colonna):p.cerchio(riga, colonna)))trovati++;
    return trovati;
}

int TrisMachine::count_sym_diag(const Tavola &p, const bool croce, const unsigned int &diagonale){
    unsigned int trovati=0;
    bool prima=(diagonale==1?true:false);
    int aux;
    for(int indice=(prima?0:(p.get_dimensione()-1));(prima?((unsigned)indice<p.get_dimensione()):(indice>=0));indice+=(prima?1:-1)){
        aux=prima?indice:(p.get_dimensione()-indice-1);
        if((croce?p.croce(indice, aux):p.cerchio(indice, aux)))trovati++;
    }
    return trovati;
}

int TrisMachine::checkAngles(const Tavola &p, int &row,const bool croce){
    if(croce && p.get_occupate()==1){
        if(!p.libera(0,0)||!p.libera(0,p.get_dimensione()-1)||!p.libera(p.get_dimensione()-1,p.get_dimensione()-1)||!p.libera(p.get_dimensione()-1,0)){
            row=0;
            return 1;
        }
    }
    return -1;
}

int TrisMachine::get_move(Tavola &p, int &row, const bool croce, ia_level ia){
    row=-1;
    int col=-1;
    if(ia==STUPIDA){
        if(check_attacks(p, row, croce, p.get_winsize(),ia)==-1)
            return find_first_free(p, row);
    }

    if((col=check_central(p, row))==-1 && (col=checkAngles(p,row,croce))==-1)
        if((col=check_all_attacks(p, row, croce, ia))==-1)
            col = find_first_free(p, row);
    return col;
}

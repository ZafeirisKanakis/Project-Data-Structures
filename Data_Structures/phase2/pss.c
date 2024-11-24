/***************************************************************
 *
 * file: pss.h
 *
 * @Version 30-11-2022
 *
 * @e-mail       hy240-list@csd.uoc.gr
 *
 * @brief   Implementation of the "pss.h" header file for the Public Subscribe System,
 * function definitions
 *
 *
 ***************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pss.h"
struct Group G[MG];
struct SubInfo *T;
int gm;
int gp;
/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @param m Size of the hash table.
 * @param p Prime number for the universal hash functions.
 *
 * @return 0 on success
 *         1 on failure
 */
int initialize(int m, int p){
    int i;
    gm =m;
    gp = p;
    for(i = 0; i < MG; i++){
        G[i].gr = NULL;
        G[i].gId = i;
        G[i].gsub = NULL;
    }
    T = calloc(m, sizeof(struct SubInfo));
    for(i = 0; i < m; i++){
        T[i].snext = NULL;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Free resources
 *
 * @return 0 on success
 *         1 on failure
 */
int free_all(void){
    return EXIT_SUCCESS;
}

I_t *BST_Insert(int iId, int itm, int *gids_arr, int size_of_gids_arr, I_t **root){
    I_t *curr = *root;
    int i;
    I_t *s = (I_t*)malloc(sizeof(I_t));
    s->iId = iId;
    s->itm = itm;
    for(i=0; i<MG; i++){
        s->igp[i] = 0; 
    }
    int k;
    for(k = 0; k < size_of_gids_arr-1; k++){
        s->igp[gids_arr[k]] = 1;
    }
    s->ilc = NULL;
    s->irc =NULL;
    s->ip = NULL;
    if(curr == NULL){
        *root = s;
        return;
    }
    while(curr != NULL){
        if(curr->iId < iId &&  curr->irc != NULL){
            curr = curr->irc;
        }else if(curr->iId > iId && curr->ilc != NULL ){
            curr = curr->ilc;
        }else if(curr->iId < iId){
            s->ip = curr;
            curr->irc = s;
            return;
        }else if(curr->iId > iId){
            s->ip = curr;
            curr->ilc = s;
            return;
        }
    }
}
void printInorder(I_t* node){
    if (node == NULL){
        return;
    }
    printInorder(node->ilc);
    printf("%d, ", node->iId);
    printInorder(node->irc);
}

void BST_Print(int *gids_arr, int num_of_gids){
    int i;
    for(i = 0; i < num_of_gids-1; i++){
        if(gids_arr[i]>63 || gids_arr[i]<0) continue;
        I_t *curr = G[gids_arr[i]].gr;
        int GId =gids_arr[i];
        printf("GROUPID = <%d>, INFOLIST = <",GId);
        printInorder(G[GId].gr);
        printf(">");
        
        printf("\n");
    }
}

int BST_LookUp(I_t *root, int iId){
    I_t *curr = root;
    while(curr != NULL){
        if(curr->iId > iId){
            curr = curr->ilc;
        }else if(curr->iId < iId){
            curr = curr->irc;
        }else if(curr->iId == iId){
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Insert info
 *
 * @param iTM Timestamp of arrival
 * @param iId Identifier of information
 * @param gids_arr Pointer to array containing the gids of the Event.
 * @param size_of_gids_arr Size of gids_arr including -1
 * @return 0 on success
 *          1 on failure
 */
int Insert_Info(int iTM,int iId,int* gids_arr,int size_of_gids_arr){
    int i, j;
    for(i = 0; i < size_of_gids_arr-1; i++){   
        if(gids_arr[i] < 0 || gids_arr[i] > 63) continue;
        j = gids_arr[i];
        if(BST_LookUp(G[j].gr, iId) == 1)continue;
        BST_Insert(iId, iTM, gids_arr, size_of_gids_arr, &G[j].gr);
    }
    return EXIT_SUCCESS;
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void SL_Insert(int id, int stm, S_t **head2, int *gids_arr, int size_of_gids_arr){
    S_t *head = *head2;
    S_t *ptrhead = head;
    S_t *s = (S_t*)malloc(sizeof(S_t));

    s->sId = id;
    s->stm = stm;
    int i;
    for(i = 0;i < MG;i++){
        s->sgp[i]=(T_t*)1;
    }
    for(i = 0;i < MG;i++){
        s->tgp[i]=(T_t*)1;
    }
    int k, j;
    for(k = 0; k < size_of_gids_arr-1; k++){
        j = gids_arr[k];    
        s->sgp[j] = NULL;
        s->tgp[j] = NULL;
    }

    s->snext = NULL;
    if(head==NULL){
        *head2 = s;
        return;
    }

    S_t *headprev = head;
    if(head->snext != NULL){
        head = head->snext;
    }
    while(head != NULL){
        int i=0;
        if(headprev->sId > id ){
            if(head->sId < id){
                headprev->snext = s;
                s->snext = head;
                return;
            }else if(headprev->snext == NULL){
                headprev->snext = s;
                s->snext = NULL;
                return;
            }else if(headprev->snext != NULL && head->snext == NULL){
                head->snext = s;
                s->snext = NULL;
                return;
            }
        }else if(i==0 && headprev->sId <= id){ 
            s->snext = headprev;
            *head2 = s;
            return;
        }
        headprev = head;
        head = head->snext;
        i++;
    }
}

int Universal_Hash_Function(int sId){
    srand(time(0));
    int a = (rand()%(gp-1)) +1;
    int b = rand()%gp;
    int k;
    k = a*sId +b;
    int j;
    j = (k % gp) % gm;
    return j;
}

void HT_Insert(int sId, int stm, int *gids_arr, int size_of_gids_arr){
    int k;
    k = Universal_Hash_Function(sId);
    SL_Insert(sId, stm, &T[k].snext, gids_arr, size_of_gids_arr);
}

/* LookUp................................................*/
int SL_LookUp(int id, S_t *head){
    while (head != NULL){
        if(head->sId == id){
            return 1;
        }
        head = head->snext;
    }
    return 0;
}

int HT_LookUp(int sId){
    int k;
    k = Universal_Hash_Function(sId);
    int p;
    p = SL_LookUp(sId, T[k].snext);
    if(p == 1){
        return 1;
    }else{
        return 0;
    }
}
/*end LookUp...........................................*/

/*Simple list inserts, LookUp..................................*/
void L_Insert(int id, Sub_t **head){
    Sub_t *ptrhead = *head;
    Sub_t *s = (Sub_t*)malloc(sizeof(Sub_t));
    /*if(L_LookUp(id, head) == 1) return;*/
    s->sId = id;
    s->snext = NULL;
    if(ptrhead == NULL){
        *head = s;
        return;
    }
    s->snext = *head;
    *head = s;
    return;
}

int L_LookUp(int id, Sub_t **head){
    Sub_t *ptrhead = *head;
    while (ptrhead != NULL){
        if(ptrhead->sId == id){
            return 1;
        }
        ptrhead = ptrhead->snext;
    }
    return 0;
}


/*end Simple list inserts, LookUp..................................*/

/*Print S Event....................................................*/

void PrintSubs (int *gids_arr, int num_of_gids){
    int j, i;
    printf("SUBSCRIBERLIST = <");
    for(i = 0; i < gm; i++){
        S_t *curr = T[i].snext;
        while(curr != NULL){
            if(i == (gm - 1) && curr->snext == NULL){
                printf("%d ", curr->sId);
            }else{
                printf("%d, ", curr->sId);
            }
            curr = curr->snext;
        }
    }
    printf(">\n");
    Sub_t *ptrhead = NULL;
    for(i = 0; i < num_of_gids-1; i++){
        if(gids_arr[i] > 63 || gids_arr[i] < 0)return;
        j = gids_arr[i];
        if(G[j].gId <= 63 || G[j].gId >= 0){
            printf("GROUPID = <%d>,", G[j].gId);
            printf("SUBLIST =<");
            ptrhead = G[j].gsub;
            while(ptrhead != NULL){
                if(ptrhead->snext == NULL){
                    printf("%d>\n", ptrhead->sId);
                }else{
                    printf("%d,", ptrhead->sId);
                }
                ptrhead =ptrhead->snext;
            }
        }

    }
    printf("\n");
}

/*end Print S Event................................................*/

/**
 * @brief Subsriber Registration
 *
 * @param sTM Timestamp of arrival
 * @param sId Identifier of subscriber
 * @param gids_arr Pointer to array containing the gids of the Event.
 * @param size_of_gids_arr Size of gids_arr including -1
 * @return 0 on success
 *          1 on failure
 */
int Subscriber_Registration(int sTM,int sId,int* gids_arr,int size_of_gids_arr){
    if(HT_LookUp(sId) == 1)return 1;
    HT_Insert(sId, sTM, gids_arr, size_of_gids_arr);

    int i, j;
    for(i = 0; i < size_of_gids_arr-1; i++){
        j = gids_arr[i];
        if(j > 63 || j < 0)return 0;
        if(L_LookUp(sId, &G[j].gsub) == 1) continue;
        L_Insert(sId, &G[j].gsub);
        /*printf("%d ", G[j].ggsub);*/
    }

    return EXIT_SUCCESS;
}
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*delete Bst tree.......................................................*/

I_t *minValueNode(I_t *node){
    I_t *current = node;
    if(current == NULL) return NULL;
    if(current->ilc == NULL && current->irc == NULL){
        return current;
    }
 
    while (current != NULL && current->ilc != NULL){
        current = current->ilc;
    }
    return current;
}




void BST_Delete(I_t **root, int iId){
    I_t *curr = *root;
    int flag2 = 0;
    int flag3 = 0;
    if(curr->ilc == NULL) flag3 = 1;
    if(curr->irc == NULL) flag2 = 1;
    if(curr->iId == iId){
        I_t *replace;
        if(curr->irc != NULL){
            replace = minValueNode(curr->irc);
        }else{
            replace = minValueNode(curr->ilc);
        }
        if(curr->irc != NULL && curr->ilc != NULL && replace->iId != curr->irc->iId){
            curr->irc->ip = replace;
            curr->ilc->ip = replace;
        }else if(curr->irc != NULL && replace->iId != curr->irc->iId){
            curr->irc->ip = replace;
        }else if(curr->ilc != NULL && replace->iId != curr->ilc->iId){
            curr->ilc->ip = replace;
        }else if(curr->ilc == NULL && curr->irc ==NULL){
            *root = NULL;
            return;
        }
        if(flag2 == 0){
            if(replace->ilc == NULL && replace->irc == NULL && curr->irc->iId != replace->iId){
                replace->irc = curr->irc;
                replace->ip->ilc = NULL;
                replace->ilc = curr->ilc;
            }else if(replace->irc != NULL && curr->iId != curr->irc->ip->iId){
                replace->irc->ip = replace->ip;
                replace->ip->ilc = replace->irc; 
                replace->irc = curr->irc;
            }else{
                replace->irc = replace->irc;
                if(flag3 != 1){
                    replace->ilc = curr->ilc;
                }
            }
        }else{
            if(replace->ilc == NULL && replace->irc == NULL && curr->ilc->iId != replace->iId){
                replace->irc = curr->irc;
                replace->ip->ilc = NULL;
                replace->ilc = curr->ilc;
            }else if(replace->irc != NULL && curr->iId != curr->ilc->ip->iId){
                replace->irc->ip = replace->ip;
                replace->ip->ilc = replace->irc;
                replace->ilc = curr->ilc;
                replace->irc = curr->irc;
            }else{
                replace->irc = replace->irc;
                replace->ilc = replace->ilc;
            }
        }
        replace->ip = NULL;
        *root = replace;
        return;
    }
    int flag =2;
    while(curr != NULL){
        if(iId > curr->iId){
            curr = curr->irc;
            flag = 0;
        }else if(iId < curr->iId){
            curr = curr->ilc;
            flag = 1;
        }else{
            break;
        }
    }
    if(curr->ilc == NULL && curr->irc == NULL){
        if(flag == 0){
            curr->ip->irc = NULL;
            curr->ip = NULL;
        }else{
            curr->ip->ilc = NULL;
        }
    }else if(curr->irc != NULL && curr->ilc == NULL){
        if(flag == 0){
            curr->ip->irc = curr->irc;
            curr->irc->ip = curr->ip;
        }else{
            curr->ip->ilc = curr->irc;
            curr->irc->ip = curr->ip;
        } 
    }else if(curr->ilc != NULL && curr->irc ==NULL){
        if(flag == 0){
            curr->ip->irc = curr->ilc;
            curr->ilc->ip = curr->ip;
        }else{
            curr->ip->ilc = curr->ilc;
            curr->ilc->ip = curr->ip;
        }
    }else if(curr->ilc != NULL && curr->irc != NULL){
        I_t *replace;
        if(curr->irc != NULL){
            replace = minValueNode(curr->irc);
        }else{
            replace = minValueNode(curr->ilc);
        }
        if(curr->irc != NULL && curr->ilc != NULL && replace->iId != curr->irc->iId){
            curr->irc->ip = replace;
            curr->ilc->ip = replace;
        }else if(curr->irc != NULL && replace->iId != curr->irc->iId){
            curr->irc->ip = replace;
        }else if(curr->ilc != NULL && replace->iId != curr->ilc->iId){
            curr->ilc->ip = replace;
        }else if(curr->ilc == NULL && curr->irc ==NULL){
            *root = NULL;
            return;
        }
        if(flag == 0){
            if((replace->ilc == NULL && replace->irc == NULL && curr->irc->iId != replace->iId)){
                replace->irc = curr->irc;
                replace->ip->ilc = NULL;
            }else{
                replace->irc = replace->irc;
            }
            curr->ip->irc = replace;
            replace->ip = curr->ip;
            replace->ilc = curr->ilc;
            curr->ilc->ip = replace;
        }else{
            if(replace->irc == NULL && replace->ilc == NULL && curr->irc->iId != replace->iId){
                replace->irc = curr->irc;
                replace->ip->ilc = NULL;
            }else if(replace->ilc != NULL){
                replace->ilc->ip = replace->ip;
                replace->ip->irc = replace->ilc; 
            }else{
                replace->irc = replace->irc;
            }
            curr->ip->ilc = replace;
            replace->ip = curr->ip;
            curr->ilc->ip = replace;
            replace->ilc = curr->ilc;
        }
    }
    
}

/*End Bst delete.......................................................*/

/*Leaf oreanted tree insert............................................*/

T_t *createTreeInfoNode(int tId, int ttm){
    T_t *s = (T_t*)malloc(sizeof(T_t));
    s->tId = tId;
    s->ttm = ttm;
    s->next = NULL;
    s->prev = NULL;
    s->tlc = NULL;
    s->trc = NULL;
    s->tp = NULL;
    return s;
}

void LO_BST_Insert(T_t **root, int tId, int ttm){
    T_t *curr = *root;
    T_t *s = createTreeInfoNode(tId, ttm);
    if(curr == NULL){
        *root = s;
        return;
    }
    int flag;
    int flag2;
    int flag3 = 0;
    int i = 0;
    while(curr != NULL){
        if(curr->ttm < ttm && curr->trc != NULL){
            curr = curr->trc;
            flag = 0;
            if(i == 0){
                flag2 = 0;
            }
        }else if(curr->ttm < ttm &&  curr->trc == NULL && i != 0){
            T_t *s2 = createTreeInfoNode(curr->tId, curr->ttm);
            s2->tp = curr->tp;
            curr->tp->trc = s2;
            s->tp = s2;
            s2->trc = s;
            s2->tlc = curr;
            curr->tp = s2;
            curr->next = s;
            s->prev = curr;
            return;
        }else if(curr->ttm < ttm && curr->trc == NULL && i == 0){
            T_t *s2 = createTreeInfoNode(curr->tId, curr->ttm);
            s2->tp = curr;
            curr->tlc = s2;
            curr->trc = s;
            s->tp = curr;
            s->prev = s2;
            s2->next = s;
            return;
        }else if(curr->ttm > ttm && curr->tlc != NULL){
            curr = curr->tlc;
            flag = 1;
            flag3 = 1;
            if(i == 0){
                flag2 = 1;
            }
        }else if(curr->ttm > ttm && curr->tlc == NULL && i == 0){
            curr->tp = s;
            s->trc = curr;
            T_t *s2 = createTreeInfoNode(s->tId, s->ttm);
            s2->tp = s;
            s->tlc = s2;
            curr->prev = s2;
            s2->next = curr;
            *root = s;
            return;
        }else if(curr->ttm > ttm && curr->tlc == NULL && i != 0){
            s->tp = curr->tp;
            T_t *s2 = createTreeInfoNode(s->tId, s->ttm);
            if(flag == 1){
                curr->tp->tlc = s;
                if(flag2 == 0){
                    curr->prev->next = s2;
                    s2->prev = curr->prev;

                }
                curr->prev = s2;
                s2->next = curr;
            }else{
                curr->tp->trc = s;
                if(flag2 == 0 || flag3 == 1){
                    curr->prev->next = s2;
                    s2->prev = curr->prev;
                }
                s2->next = curr;
                curr->prev = s2;
            }
            curr->tp = s;
            s->trc = curr;
            s2->tp = s;
            s->tlc = s2;
            
            return;
        }
        i++;
    }
}

/*End of Leaf oreanted tree insert......................................*/
void ExecutePrune(int gId, int tm, I_t **oGroot, I_t *root){
    if (root == NULL){
        return;
    }
    ExecutePrune(gId, tm, &*oGroot, root->ilc);
    if(root->itm <= tm){
        Sub_t *curr = G[gId].gsub;
        while(curr != NULL){
            int id = curr->sId;
            int placeInHash = Universal_Hash_Function(id);
            S_t *currs = T[placeInHash].snext;
            while(currs != NULL){
                if(id == currs->sId){
                    break;
                }
                if(currs->snext == NULL) break;
                currs->snext;
            }
            LO_BST_Insert(&currs->tgp[gId],root->iId, root->itm);
            curr =curr->snext;
        }
        /*BST_Delete(&*oGroot, root->iId);*/
    }
    ExecutePrune(gId, tm, &*oGroot, root->irc);
}

BSDelete(int gId, int tm, I_t **oGroot, I_t *root){
    if (root == NULL){
        return;
    }
    BSDelete(gId, tm, &*oGroot, root->ilc);
    if(root->itm <= tm){
        BST_Delete(&*oGroot, root->iId);
    }
    BSDelete(gId, tm, &*oGroot, root->irc);
}



/**
 * @brief Prune Information from server and forward it to client
 *
 * @param tm Information timestamp of arrival
 * @return 0 on success
 *          1 on failure
 */
int Prune(int tm){
    int i, GId;
    Sub_t *ptrhead;
    for(i = 0; i < MG; i++){
        ExecutePrune(i, tm, &G[i].gr, G[i].gr);
        BSDelete(i, tm, &G[i].gr, G[i].gr);
        GId =G[i].gId;
        printf("GROUPID = <%d>, INFOLIST = <",GId);
        printInorder(G[GId].gr);
        printf(">,");
        printf(" SUBLIST = <");
        ptrhead = G[i].gsub;
        if(ptrhead == NULL) printf(">");
        while(ptrhead != NULL){
            if(ptrhead->snext == NULL){
                printf("%d>", ptrhead->sId);
            }else{
                printf("%d,", ptrhead->sId);
            }
            ptrhead =ptrhead->snext;
        }

        printf("\n");
    }
    S_t *head;
    int j;
    for(i = 0; i < gm; i++){
        head = T[i].snext;
        while(head != NULL){
            printf("SUBSCRIBERID = <%d>, GROUPLIST =\n", head->sId);
            for(j = 0; j < MG; j++){
                if(head->tgp[j] != (T_t*)1){
                    printf("<%d>", j);
                    T_t *headLoTree = head->tgp[j];
                    while(headLoTree != NULL){
                        if(headLoTree->tlc == NULL) break;
                        headLoTree= headLoTree->tlc;
                    }
                    printf(", TREELIST<");
                    while(headLoTree != NULL){
                        if(headLoTree->next == NULL){
                            printf("%d(%d)",headLoTree->tId, j);
                        }else{
                            printf("%d(%d), ",headLoTree->tId,j);
                        }
                        headLoTree = headLoTree->next;
                    }
                    printf(">");
                    printf("\n");
                }
            }
            head = head->snext;
        }
    }
    return EXIT_SUCCESS;
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/**
 * @brief Consume Information for subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Consume(int sId){
    int j, i;
    j = Universal_Hash_Function(sId);
    T_t *currinfo;
    T_t *infolist;
    S_t *sub;
    sub = T[j].snext;
    if(HT_LookUp(sId) == 0) return 0;
    while(sub != NULL){
        if(sub->sId == sId) break;
        sub = sub->snext;
    } 
    for(i = 0; i < MG; i++){
        if(sub->sgp[i] == NULL){
            if(sub->tgp[i] != NULL && sub->tgp[i] != (T_t*)1){
                currinfo = sub->tgp[i];
                while(currinfo != NULL){
                    if(currinfo->tlc == NULL){
                        sub->sgp[i] = currinfo;
                    }
                    currinfo = currinfo->tlc;
                }
            }else if(sub-> tgp[i] == NULL){
                sub->sgp[i] = NULL;
            }else if(sub->tgp[i] == (T_t*)1){
                continue;
            }
        }else if(sub->sgp[i] == (T_t*)1){
            continue;
        }
        infolist = sub->sgp[i];
        printf("GROUPID = <%d>, TREELIST = <", i);
        while(infolist != NULL){
            if(infolist->next == NULL){
                printf("%d",infolist->tId);
                sub->sgp[i] = infolist;
            }else{
                printf("%d, ", infolist->tId);
            }
            infolist = infolist->next;
        }
        printf(">,");
        if(sub->sgp[i] == NULL){
            printf("NEWSGP = <>\n");
        }else{
            printf("NEWSGP = <%d>\n", sub->sgp[i]->tId);
        }
    }
    return EXIT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void SL_Delete(S_t **head2, int id){
    S_t *head = *head2;
    S_t *ptrhead = head;
    S_t *headprev = head;
    

    int i=0;
    while(head!=NULL){  
        
        if(head->sId == id){
            if(head->snext == NULL && headprev->snext == NULL){
                *head2 = NULL;
                return;
            }else if(head->snext == NULL && headprev->snext != NULL){
                headprev->snext =NULL;
                return;
            }else if(head->snext != NULL && headprev->snext != NULL && i!=0){
                headprev->snext = head->snext;
                return;
            }else{
                *head2 = head->snext;
                return;
            }
        }
        i++;
        headprev =head;
        head = head->snext;
    }
}




void HT_Delete(int sId){
    int k;
    k = Universal_Hash_Function(sId);
    SL_Delete(&T[k].snext, sId);
}

/*delete list L..................................................................*/
void L_Delete(int id, Sub_t **head1){
    Sub_t *ptrhead = *head1;
    Sub_t *prevhead = *head1;
    Sub_t *head = *head1;
    int i = 0;
    while(head != NULL){
        if(head->sId == id){
            if(i == 0){
                *head1 = head->snext;
                return;
            }else if(head->snext != NULL){
                prevhead->snext = head->snext;
                return;
            }else if(head->snext == NULL){
                prevhead->snext = NULL;
                return; 
            }
        }
        prevhead = head;
        head = head->snext;
        i++;
    }
}


/*End delete list L..............................................................*/
/**
 * @brief Delete subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Delete_Subscriber(int sId){
    if(HT_LookUp(sId) == 1){
        HT_Delete(sId);
    }
    int i;
    printf("SUBSCRIBERLIST = <");
    for(i = 0; i < gm; i++){
        S_t *curr = T[i].snext;
        while(curr != NULL){
            if(i == (gm - 1) && curr->snext == NULL){
                printf("%d ", curr->sId);
            }else{
                printf("%d, ", curr->sId);
            }
            curr = curr->snext;
        }
    }
    printf("\n");
    int j;
    Sub_t *ptrhead;
    for(i = 0; i < MG; i++){
            printf("GROUPID = <%d>,", G[i].gId);
            printf("SUBLIST =<");
        if(L_LookUp(sId, &G[i].gsub) == 1){
            L_Delete(sId, &G[i].gsub);
            ptrhead = G[i].gsub;
            while(ptrhead != NULL){
                if(ptrhead->snext == NULL){
                    printf("%d", ptrhead->sId);
                }else{
                    printf("%d,", ptrhead->sId);
                }
                ptrhead =ptrhead->snext;
            }
        }
        printf(">\n");
    }
    printf(">\n");
    return EXIT_SUCCESS;
}
/**
 * @brief Print Data Structures of the system
 *
 * @return 0 on success
 *          1 on failure
 */
int Print_all(void){
    int i, GId;
    Sub_t *ptrhead;
    int no_of_groups = 0;
    for(i = 0; i < MG; i++){
        GId =G[i].gId;
        printf("GROUPID = <%d>, INFOLIST = <",GId);
        printInorder(G[GId].gr);
        if(G[GId].gr != NULL) no_of_groups++;
        printf(">,");
        printf(" SUBLIST = <");
        ptrhead = G[i].gsub;
        if(ptrhead == NULL) printf(">");
        while(ptrhead != NULL){
            if(ptrhead->snext == NULL){
                printf("%d>", ptrhead->sId);
            }else{
                printf("%d,", ptrhead->sId);
            }
            ptrhead =ptrhead->snext;
        }

        printf("\n");
    }
    int no_of_subscribers = 0;
    printf("SUBSCRIBERLIST = <");
    for(i = 0; i < gm; i++){
        S_t *curr = T[i].snext;
        while(curr != NULL){
            no_of_subscribers++;
            if(i == (gm - 1) && curr->snext == NULL){
                printf("%d >", curr->sId);
            }else{
                printf("%d, ", curr->sId);
            }
            curr = curr->snext;
        }
    }
    printf(">\n");
    S_t *head;
    int j;
    for(i = 0; i < gm; i++){
        head = T[i].snext;
        while(head != NULL){
            printf("SUBSCRIBERID = <%d>, GROUPLIST =\n", head->sId);
            for(j = 0; j < MG; j++){
                if(head->tgp[j] != (T_t*)1){
                    printf("<%d>", j);
                    T_t *headLoTree = head->tgp[j];
                    while(headLoTree != NULL){
                        if(headLoTree->tlc == NULL) break;
                        headLoTree= headLoTree->tlc;
                    }
                    printf(", TREELIST<");
                    while(headLoTree != NULL){
                        if(headLoTree->next == NULL){
                            printf("%d(%d)",headLoTree->tId, j);
                        }else{
                            printf("%d(%d), ",headLoTree->tId,j);
                        }
                        headLoTree = headLoTree->next;
                    }
                    printf(">");
                    printf("\n");
                }
            }
            head = head->snext;
        }
    }
    printf("NO_GROUPS = <%d>, NO_SUBSCRIBERS = <%d>", no_of_groups, no_of_subscribers);
    printf("\n");

    return EXIT_SUCCESS;
}

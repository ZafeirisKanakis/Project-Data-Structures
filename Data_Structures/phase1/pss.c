
#include <stdio.h>
#include <stdlib.h>

#include "pss.h"

struct Group G[MG];
struct SubInfo *S;
/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 0 on success
 *         1 on failure
 */
/*complexity = MG*/
int initialize(void){
    int i;
    for(i = 0; i < MG; i++){
        G[i].gfirst = NULL;
        G[i].glast = NULL;
        G[i].gId = i;
        G[i].ggsub = NULL;
    }
    return EXIT_SUCCESS;
}

/**
 * 
 * @brief Free resources
 *
 * @return 0 on success
 *         1 on failure
 */
int free_all(void){
    return EXIT_SUCCESS;
}

int DL_LookUp(I_t *head, int id){
    I_t *curr = head;
    while (curr != NULL){
        if(curr->iId == id){
            return 1;
        }
        curr = curr->inext;
    }
    return 0;
}

/*complexity == MG^2 + n
    n == size of info list
*/
void DL_Insert(int id, int stm,int *gids_arr, int size_of_gids_arr,  I_t **gfirst, I_t **glast){
    I_t *ptrhead = *gfirst;
    I_t *head = *gfirst;
    I_t *s = (I_t*)malloc(sizeof(I_t));
    s->iId = id;
    s->itm = stm;
    int i, k;
    for(i=0; i<MG; i++){
        s->igp[i] = 0;
    }
    for(k = 0; k < size_of_gids_arr-1; k++){
        s->igp[gids_arr[k]] = 1;
    }
    s->inext = NULL;
    s->iprev = NULL;
    if(*gfirst==NULL){
        *glast = s;
        *gfirst = s;
        return;
    }

    I_t *headprev = head;
    if(head->inext != NULL){
        head = head->inext;
    }
    
    i=0;
    while (head != NULL){
        if(headprev->itm > stm){
            if(head->itm <= stm){
                headprev->inext = s;
                s->iprev = headprev;
                s->inext = head;
                head->iprev = s; 
                return;
            }else if(head->inext == NULL){
                head->inext = s;
                s->inext = NULL;
                s->iprev = head;
                *glast = s;
                return;
            }
        }else if(i==0 && headprev->itm <= stm){
            s->inext = headprev;
            s->iprev = NULL;
            headprev->iprev = s; 
            *gfirst = s;
            return;
        }
        headprev = head;
        head = head->inext;
        i++;
    }
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
/*complexity == (num_of_gids-1)*n
  n == size of info list
*/ 
void PrTheList(int *gids_arr, int num_of_gids){
    int i, gid;
    for(i = 0; i < num_of_gids-1; i++){
        if(gids_arr[i]>63 || gids_arr[i]<0) continue;
        I_t *curr = G[gids_arr[i]].gfirst;
        int GId =gids_arr[i];
        printf("GROUPID = <%d>, INFOLIST = <",GId);
    
        while(curr != NULL){
            if(curr->inext == NULL){
                printf("%d> \n", curr->iId);
            }else{
                printf("%d,", curr->iId);
            }
            curr=curr->inext;
        }
        /*DL_Print(G[gids_arr[i]].gfirst, gids_arr[i]);*/
    }
}

int Insert_Info(int iTM,int iId,int* gids_arr,int size_of_gids_arr){
    int i, j;
    for(i = 0; i < size_of_gids_arr-1; i++){   
        if(gids_arr[i] < 0 || gids_arr[i] > 63) continue;
        j = gids_arr[i];
        if(DL_LookUp(G[j].gfirst, iId) == 1)continue;
        
        DL_Insert(iId, iTM, gids_arr, size_of_gids_arr, &G[j].gfirst, &G[j].glast );
    }
    return EXIT_SUCCESS;
}

/*complexity == MG + size_of_gids_arr-1 + n
  n == the size of the subInfo list
*/
void SL_Insert(int id, int stm, int *gids_arr, int size_of_gids_arr){
    S_t *head = S;
    S_t *s = (S_t*)malloc(sizeof(S_t));
    int i;
    s->sId = id;
    s->stm = stm;
    for(i = 0;i < MG;i++){
        s->sgp[i]=(I_t*)1;
    }
    int k;
    int j;
    for(k = 0; k < size_of_gids_arr-1; k++){
        j = gids_arr[k];    
        s->sgp[j] = G[j].gfirst;
    }
    s->snext = NULL;
    if(head==NULL){
        S = s;
        return;
    }

    S_t *headprev = head;
    if(head->snext != NULL){
        head = head->snext;
    }
    while(head != NULL){
        int i=0;
        if(headprev->stm > stm ){
            if(head->stm < stm){
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
        }else if(i==0 && headprev->stm <= stm){ 
            s->snext = headprev;
            S = s;
            return;
        }
        headprev = head;
        head = head->snext;
        i++;
    }
}

/*complexity == n
  n == size of the SubInfo list
*/
int SL_LookUp(int id){
    S_t *head = S;
    while (head != NULL){
        if(head->sId == id){
            return 1;
        }
        head = head->snext;
    }
    return 0;
    
}
/*complexity == 1*/
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


/*complexity == n
  n == size of the subscriber list
*/
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
 /*complexity == n + (num_of_gids-1)*k
   n == SubInfo list
   k == Subscriber list
 */
void PrTheListS12(int *gids_arr, int num_of_gids){
    printf("SUBSCRIBERLIST = <");
    int i, j;
    S_t *head = S;
    while (head != NULL){
        if(head->snext == NULL){
            printf("%d>", head->sId);
        }else{
            printf("%d,", head->sId);
        }
        head = head->snext;
    }
    printf("\n");
    Sub_t *ptrhead = NULL;
    
    for(i = 0; i < num_of_gids-1; i++){
        if(gids_arr[i] > 63 || gids_arr[i] < 0)return;
        j = gids_arr[i];
        if(G[j].gId <= 63 || G[j].gId >= 0){
            printf("GROUPID = <%d>,", G[j].gId);
            printf("SUBLIST =<");
            ptrhead = G[j].ggsub;
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
    int i, j;
    if(SL_LookUp(sId) == 1)return 1;
    /*if(sId > 63)return 1;*/
    SL_Insert(sId, sTM, gids_arr, size_of_gids_arr);

    for(i = 0; i < size_of_gids_arr-1; i++){
        j = gids_arr[i];
        if(j > 63 || j < 0)return 0;
        if(L_LookUp(sId, &G[j].ggsub) == 1) continue;
        L_Insert(sId, &G[j].ggsub);
        /*printf("%d ", G[j].ggsub);*/
    }
    
    /*printf("%d ", S->sgp[2]);*/
    return EXIT_SUCCESS;
}
/**
 * @brief Consume Information for subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */

/*complexity == n + MG*k
  k == size of info list
  n == SubInfo list
*/
int Consume(int sId){
    S_t *currsub = S;
    I_t *first;
    I_t *currinfo;
    int i, j;
    if(SL_LookUp(sId) == 0)return 0;
    while(currsub != NULL){
        if(currsub->sId == sId) break;
        currsub = currsub->snext;
    }
    for(i = 0; i < MG; i++){
        j = 0;
        if(currsub->sgp[i] != (I_t*)1){
            first = currsub->sgp[i];
            currinfo = G[i].gfirst;
            printf("GROUPID = <%d>, INFOLIST = <", i);
            while(currinfo != first){
                if(currinfo->inext == first){
                    printf("%d>, ", currinfo->iId);
                    currsub->sgp[i] = G[i].gfirst;
                }else{
                    printf("%d,", currinfo->iId);
                }
                j++;
                currinfo = currinfo->inext;
            }
            if(j == 0){
                printf(">, ");
            }
            if(currsub->sgp[i] != NULL){
                printf("NEWSGP = <%d>\n", currsub->sgp[i]->iId);
            }else{
                printf("NEWSGP = <>\n");
        }
        }else if(currsub->sgp[i] == NULL){
            printf("no new info\n");
            currsub->sgp[i] = G[i].gfirst;
        }
    }
    printf("\n");
    return EXIT_SUCCESS;
}


/*complexity == n
  n == size of subscriber list
*/
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

/*complexity == n
  n == SubInfo list
*/

void SL_Delete(int id){
    S_t *ptrhead = S;
    S_t *headprev = S;
    S_t *head = S;
    

    int i;
    i=0;
    while(head!=NULL){  
        
        if(head->sId == id){
            if(head->snext == NULL && headprev->snext == NULL){
                S = NULL;
                return;
            }else if(head->snext == NULL && headprev->snext != NULL){
                headprev->snext =NULL;
                return;
            }else if(head->snext != NULL && headprev->snext != NULL && i!=0){
                headprev->snext = head->snext;
                return;
            }else{
                S = head->snext;
                return;
            }
        }
        i++;
        headprev =head;
        head = head->snext;
    }
}

/**
 * @brief Delete subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */


int Delete_Subscriber(int sId){
    if(SL_LookUp(sId) == 1){
        SL_Delete(sId);
    }
    int i;
    printf("SUBSCRIBERLIST = <");
    S_t *head = S;
    Sub_t * ptrhead;
    while (head != NULL){
        if(head->snext == NULL){
            printf("%d>\n", head->sId);
        }else{
            printf("%d,", head->sId);
        }
        head = head->snext;
    }
    int k = 0;
    for(i = 0; i < MG; i++){
        if(L_LookUp(sId, &G[i].ggsub) == 1){
            L_Delete(sId, &G[i].ggsub);
            printf("GROUPID = <%d>,", G[i].gId);
            printf("SUBLIST =<");
            ptrhead = G[i].ggsub;
            if(G[i].ggsub == NULL)printf(">");
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
    }
    
    printf("\n");
    return EXIT_SUCCESS;
}
/**
 * @brief Print Data Structures of the system
 *
 * @return 0 on success
 *          1 on failure
 */

/*complexity == MG*n + k + l+ k*MG
  l == size of SubInfo list 
  n == size of info list
  k == size of subscriber list
*/
int Print_all(void){
    int i;
    int j, k, no_of_groups, no_of_subscribers;
    no_of_groups = 0;
    for(i = 0; i < MG; i++){
        j=0;
        I_t *curr2 = G[i].gfirst;
        printf("GROUPID = <%d>, INFOLIST = <", G[i].gId);
        while(curr2 != NULL){
            j++;
            if(curr2->inext == NULL){
                no_of_groups++;
                printf("%d>,  SUBLIST = <", curr2->iId);
            }else{
                no_of_groups++;
                printf("%d,", curr2->iId);
            }
            curr2=curr2->inext;
        }
        if (curr2 == NULL && j == 0){
            printf(" >,  SUBLIST = <");
        }

        Sub_t *currsublist = G[i].ggsub;
        j=0;
        while(currsublist != NULL){
            j++;
            if(currsublist->snext == NULL){
                printf("%d>", currsublist->sId);
            }else{
                printf("%d,", currsublist->sId);
            }
            currsublist = currsublist->snext;
        }
        if(currsublist == NULL && j == 0){
            printf(">");
        }
        
        printf("\n");
    }

    S_t *currsubscriber = S;
    printf("SUBSCRIBERLIST = <");
    while(currsubscriber != NULL){
        if(currsubscriber->snext == NULL){
            printf("%d>", currsubscriber->sId);
        }else{
            printf("%d,", currsubscriber->sId);
        }
        currsubscriber = currsubscriber->snext;
    }
    printf("\n");

    currsubscriber = S;
    while(currsubscriber != NULL){
        no_of_subscribers++;
        printf("SUBSCRIBERID = <%d>, GROUPLIST = <", currsubscriber->sId);
        for(k = 0; k < MG; k++){
            if(currsubscriber->sgp[k] != (I_t*)1){
                printf("%d,", G[k].gId);
            }
            if(k == 63){
                printf(">");
            }
        }
        currsubscriber = currsubscriber->snext;
        printf("\n");
    }
    printf("NO_GROUPS = <%d>, NO_SUBSCRIBERS = <%d>", no_of_groups, no_of_subscribers);
    printf("\n");

    return EXIT_SUCCESS;
}

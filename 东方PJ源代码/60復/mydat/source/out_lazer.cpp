#include "../include/GV.h"
#include <math.h>
#include <stdio.h>

/* ２次元ベクトル */
typedef struct {
    double x, y;
} Vector2_t;

/* diff ← ベクトル p - q */
void Vector2Diff(Vector2_t *diff, const Vector2_t *p, const Vector2_t *q){
    diff->x = p->x - q->x;
    diff->y = p->y - q->y;
}

/* ベクトル p と q の内積 */
double Vector2InnerProduct(const Vector2_t *p, const Vector2_t *q){
    return p->x * q->x + p->y * q->y;
}

/* ベクトル p と q の外積 */
double Vector2OuterProduct(const Vector2_t *p, const Vector2_t *q){
    return p->x * q->y - p->y * q->x;
}

//点と線分との距離を求める
double get_distance(double x, double y, double x1, double y1, 
                    double x2, double y2){
    double dx,dy,a,b,t,tx,ty;
    double distance;
    dx = (x2 - x1); dy = (y2 - y1);
    a = dx*dx + dy*dy;
    b = dx * (x1 - x) + dy * (y1 - y);
    t = -b / a;
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    tx = x1 + dx * t;
    ty = y1 + dy * t;
    distance = sqrt((x - tx)*(x - tx) + (y - ty)*(y - ty));
    return distance;
}

//点と点との距離を返す
double get_pt_and_pt(pt_t p1, pt_t p2){
    return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}

//点が円の中にあるかどうか。0:なし　1:あり
double question_point_and_circle(pt_t p, pt_t rp,double r){
    double dx=p.x-rp.x,dy=p.y-rp.y;
    if(dx*dx + dy*dy < r*r)    return 1;
    else                    return 0;
}

//入れ替え
void swap_double(double *n, double *m){
    double t=*m;
    *m=*n;*n=t;
}

//３点から角度を返す
double get_sita(pt_t pt0,pt_t pt1,pt_t rpt){
    /* ベクトル C→P と C→Q のなす角θおよび回転方向を求める．*/
    Vector2_t c, p, q; /* 入力データ */
    Vector2_t cp;      /* ベクトル C→P */
    Vector2_t cq;      /* ベクトル C→Q */
    double s;          /* 外積：(C→P) × (C→Q) */
    double t;          /* 内積：(C→P) ・ (C→Q) */
    double theta;      /* θ (ラジアン) */

    /* c，p，q を所望の値に設定する．*/
    c.x = pt0.x;    c.y = pt0.y;
    p.x = pt1.x;    p.y = pt1.y;
    q.x = rpt.x;    q.y = rpt.y;

    /* 回転方向および角度θを計算する．*/
    Vector2Diff(&cp, &p, &c);          /* cp ← p - c   */
    Vector2Diff(&cq, &q, &c);          /* cq ← q - c   */
    s = Vector2OuterProduct(&cp, &cq); /* s ← cp × cq */
    t = Vector2InnerProduct(&cp, &cq); /* t ← cp ・ cq */
    theta = atan2(s, t);
    return theta;
}

//長方形と円との当たりを判定する
int hitjudge_square_and_circle(pt_t pt[4], pt_t rpt, double r){
    int i;
    double x=rpt.x,y=rpt.y;
	double theta,theta2;

    /*円の中に長方形の４点のうちどれかがあるかどうか判定*/
    for(i=0;i<4;i++){
        if(question_point_and_circle(pt[i],rpt,r)==1)
            return 1;
    }
    /*ここまで*/

    /*長方形の中に物体が入り込んでいるかどうかを判定判定*/

    theta =get_sita(pt[0],pt[1],rpt);//3点の成す角1
    theta2=get_sita(pt[2],pt[3],rpt);//3点の成す角2

    if(0<=theta && theta<=PI/2 && 0<=theta2 && theta2<=PI/2)
        return 1;

    /*ここまで*/

    /*線分と点との距離を求める*/
    for(i=0;i<4;i++){
        if(get_distance(rpt.x,rpt.y,pt[i].x,pt[i].y,pt[(i+1)%4].x,pt[(i+1)%4].y)<r)
            return 1;
    }
    /*ここまで*/
    return 0;//どこにもヒットしなかったらぶつかっていない
}


int out_lazer(){
    int i,j;
    pt_t sqrp[4],rpt={ch.x,ch.y};//長方形の4点と円の中心
    //レーザー分ループ
    for(i=0;i<LAZER_MAX;i++){
        //レーザーが登録されていて、当たり判定をする設定なら
        if(lazer[i].flag>0 && lazer[i].hantei!=0){
            for(j=0;j<4;j++){//レーザーの4点を設定
                sqrp[j].x=lazer[i].outpt[j].x;
                sqrp[j].y=lazer[i].outpt[j].y;
            }
            //長方形と円との接触判定
            if(hitjudge_square_and_circle(sqrp,rpt,CRANGE))
                return 1;
        }
    }
    return 0;
}
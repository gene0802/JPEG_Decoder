# include "data.cpp"
# include "bitmap_image.hpp"
# define M_PI 3.14159265358979323846
# define squ2_1 0.7071067811865476f
const size_t z[8][8]={
    { 0,  1,  5,  6, 14, 15, 27, 28},
    { 2,  4,  7, 13, 16, 26, 29, 42},
    { 3,  8, 12, 17, 25, 30, 41, 43},
    { 9, 11, 18, 24, 31, 40, 44, 53},
    { 10, 19, 23, 32, 39, 45, 52, 54},
    { 20, 22, 33, 38, 46, 51, 55, 60},
    { 21, 34, 37, 47, 50, 56, 59, 61},
    { 35, 36, 48, 49, 57, 58, 62, 63}};
const float cosTable[8][8]=
{{1.0000000000,0.9807852804,0.9238795325,0.8314696123,0.7071067812,0.5555702330,0.3826834324,0.1950903220},
{1.0000000000,0.8314696123,0.3826834324,-0.1950903220,-0.7071067812,-0.9807852804,-0.9238795325,-0.5555702330},
{1.0000000000,0.5555702330,-0.3826834324,-0.9807852804,-0.7071067812,0.1950903220,0.9238795325,0.8314696123},
{1.0000000000,0.1950903220,-0.9238795325,-0.5555702330,0.7071067812,0.8314696123,-0.3826834324,-0.9807852804},
{1.0000000000,-0.1950903220,-0.9238795325,0.5555702330,0.7071067812,-0.8314696123,-0.3826834324,0.9807852804},
{1.0000000000,-0.5555702330,-0.3826834324,0.9807852804,-0.7071067812,-0.1950903220,0.9238795325,-0.8314696123},
{1.0000000000,-0.8314696123,0.3826834324,0.1950903220,-0.7071067812,0.9807852804,-0.9238795325,0.5555702330},
{1.0000000000,-0.9807852804,0.9238795325,-0.8314696123,0.7071067812,-0.5555702330,0.3826834324,-0.1950903220}};

class Decoder{
    public:
        Data *data;

        Decoder(Data *d){
            data = d;
        }

        void toRGB(){

            const vector < vector < YCbCr> > & YUV = data->YUV;
            vector < vector < RGB > > & color = data->color;

            bitmap_image img(data->width,data->height);
            int r,g,b;
            for(int i=0;i<data->height;i++){
                for(int j=0;j<data->width;j++){
                    r = YUV[i][j].r + 1.402* YUV[i][j].b + 128.0;
                    g = YUV[i][j].r - 0.34414* YUV[i][j].g - 0.71414*YUV[i][j].b +128.0;
                    b = YUV[i][j].r + 1.772* YUV[i][j].g +128.0;
                    
                    r = (r < 255) ? r :255;
                    color[i][j].r = (r > 0) ? r :0;

                    g = (g < 255) ? g :255;
                    color[i][j].g = (g > 0) ? g :0;

                    b = (b < 255) ? b :255;
                    color[i][j].b = (b > 0) ? b :0;
                }
            }
            for(int i=0;i<data->width;i++){
                for(int j=0;j<data->height;j++){
                    img.set_pixel(i,j,color[j][i].r,color[j][i].g,color[j][i].b); 
                }
            }
            img.save_image("./teatime.bmp");
        }
        void reSample(){
            vector < vector < YCbCr > > & YUV = data->YUV;
            const vector < vector < Block > > & Y = data->Y;
            const vector < vector < Block > > & Cb = data->Cb;
            const vector < vector < Block > > & Cr = data->Cr;

            float modV_Y = data->Y_inform.sRateV/data->sRateV_Max;
            float modV_Cb = data->Cb_inform.sRateV/data->sRateV_Max;
            float modV_Cr = data->Cr_inform.sRateV/data->sRateV_Max;

            float modH_Y = data->Y_inform.sRateH/data->sRateH_Max;
            float modH_Cb = data->Cb_inform.sRateH/data->sRateH_Max;
            float modH_Cr = data->Cr_inform.sRateH/data->sRateH_Max;

            for(int i=0;i<data->height;i++){
                int i_Y= i * data->Y_inform.sRateV/data->sRateV_Max;
                int i_Cb = i * data->Cb_inform.sRateV/data->sRateV_Max;
                int i_Cr = i * data->Cr_inform.sRateV/data->sRateV_Max;
                for(int j=0;j<data->width;j++){
                    int j_Y= j * data->Y_inform.sRateH/data->sRateH_Max;
                    int j_Cb = j * data->Cb_inform.sRateH/data->sRateH_Max;
                    int j_Cr = j * data->Cr_inform.sRateH/data->sRateH_Max;

                    YUV[i][j].r = Y [i_Y/8][j_Y/8].v[i_Y%8][j_Y%8];
                    YUV[i][j].g = Cb[i_Cb/8][j_Cb/8].v[i_Cb%8][j_Cb%8];
                    YUV[i][j].b = Cr[i_Cr/8][j_Cr/8].v[i_Cr%8][j_Cr%8];
                }
            }

        }
        void invDCT(){

            Block newB;
            int Y_row = data->Y.size();
            int Y_col = data->Y[0].size();
            int Cb_row = data->Cb.size();
            int Cb_col = data->Cb[0].size();
            int Cr_row = data->Cr.size();
            int Cr_col = data->Cr[0].size();



            // Parse all Y
            for (int m=0;m<Y_row;m++)
                for(int n=0;n<Y_col;n++){
                    Block *b = &data->Y[m][n];

                    for(int l=0; l<8; l++){
                        for(int k=0; k<8; k++){
                            float sum = 0.0;
                            // DC term
                                sum += 0.5f * (*b).v[0][0] ;
                            // first row term
                            for(int v=1;v<8;v++)
                                sum += squ2_1* (*b).v[0][v] * cosTable[k][v];
                            // fisrt col term
                            for(int u=1;u<8;u++)
                                sum += squ2_1* (*b).v[u][0] * cosTable[l][u];
                            // left term
                            for(int u=1;u<8;u++)
                                for(int v=1;v<8;v++)
                                    sum += (*b).v[u][v]*cosTable[l][u]*cosTable[k][v];;
                            newB.v[l][k] = (short)round(0.25*sum);
                        }
                    }
                    *b = newB;
                }
            //Parse all Cb
            for (int m=0;m<Cb_row;m++)
                for(int n=0;n<Cb_col;n++){

                    Block *b = &data->Cb[m][n];

                    for(int l=0; l<8; l++){
                        for(int k=0; k<8; k++){
                            float sum = 0.0;
                            // DC term
                                sum += 0.5f * (*b).v[0][0] ;
                            // first row term
                            for(int v=1;v<8;v++)
                                sum += squ2_1* (*b).v[0][v]*cosTable[k][v];
                            // fisrt col term
                            for(int u=1;u<8;u++)
                                sum += squ2_1* (*b).v[u][0]*cosTable[l][u];
                            // left term
                            for(int u=1;u<8;u++)
                                for(int v=1;v<8;v++)
                                    sum += (*b).v[u][v]*cosTable[l][u]*cosTable[k][v];;
                            newB.v[l][k] = (short)round(0.25*sum);
                        }
                    }
                    *b = newB;
                }
            //Parse all Cr
            for (int m=0;m<Cr_row;m++)
                for(int n=0;n<Cr_col;n++){

                    Block *b = &data->Cr[m][n];

                    for(int l=0; l<8; l++){
                        for(int k=0; k<8; k++){
                            float sum = 0.0;
                            // DC term
                                sum += 0.5f * (*b).v[0][0] ;
                            // first row term
                            for(int v=1;v<8;v++)
                                sum += squ2_1* (*b).v[0][v]*cosTable[k][v];
                            // fisrt col term
                            for(int u=1;u<8;u++)
                                sum += squ2_1* (*b).v[u][0]*cosTable[l][u];
                            // left term
                            for(int u=1;u<8;u++)
                                for(int v=1;v<8;v++)
                                    sum += (*b).v[u][v]*cosTable[l][u]*cosTable[k][v];;
                            newB.v[l][k] = (short)round(0.25*sum);
                        }
                    }
                    *b = newB;
                }
                
        }
        void invZigZag(){

            int row = 1+(data->height - 1)/data->height_MCU;
            int col = 1+(data->width - 1)/data->width_MCU;

            vector <vector <Block>> newY(row * (data->Y_inform.sRateV),vector<Block>(col* data->Y_inform.sRateH));
            vector <vector <Block>> newCb(row * (data->Cb_inform.sRateV),vector<Block>(col* data->Cb_inform.sRateH));
            vector <vector <Block>> newCr(row * (data->Cr_inform.sRateV),vector<Block>(col* data->Cr_inform.sRateH));

                for (int i=0; i<row;i++){
                    for (int j=0; j<col;j++){
                        // Parse all Y
                        for (int m=0;m<data->Y_inform.sRateV;m++)
                            for(int n=0;n<data->Y_inform.sRateH;n++){
                                int Y_index = m*data->Y_inform.sRateV+n;
                                for(int l=0;l<8;l++)
                                    for(int k=0;k<8;k++)
                                        newY[data->Y_inform.sRateV*i+m][data->Y_inform.sRateH*j+n].v[l][k] = data->Y[data->Y_inform.sRateV*i+m][data->Y_inform.sRateH*j+n].v[z[l][k]/8][z[l][k]%8];
                            }
                        //Parse all Cb
                        for (int m=0;m<data->Cb_inform.sRateV;m++)
                            for(int n=0;n<data->Cb_inform.sRateH;n++){
                                int Cb_index = m*data->Cb_inform.sRateH+n;
                                for(int l=0;l<8;l++)
                                    for(int k=0;k<8;k++)
                                        newCb[data->Cb_inform.sRateV*i+m][data->Cb_inform.sRateH*j+n].v[l][k] = data->Cb[data->Cb_inform.sRateV*i+m][data->Cb_inform.sRateH*j+n].v[z[l][k]/8][z[l][k]%8];
                            }
                        //Parse all Cr
                        for (int m=0;m<data->Cr_inform.sRateV;m++)
                            for(int n=0;n<data->Cr_inform.sRateH;n++){
                                int Cr_index = m*data->Cr_inform.sRateH+n;
                                for(int l=0;l<8;l++)
                                    for(int k=0;k<8;k++)
                                        newCr[data->Cr_inform.sRateV*i+m][data->Cr_inform.sRateH*j+n].v[l][k] = data->Cr[data->Cr_inform.sRateV*i+m][data->Cr_inform.sRateH*j+n].v[z[l][k]/8][z[l][k]%8];
                            }
                    }
                }

            data->Y=newY;
            data->Cb=newCb;
            data->Cr=newCr;
        }
        void invQuantize(){

                int row = 1+(data->height - 1)/data->height_MCU;
                int col = 1+(data->width - 1)/data->width_MCU;
                for (int i=0; i<row;i++){
                    for (int j=0; j<col;j++){
                        // Parse all Y
                        for (int m=0;m<data->Y_inform.sRateV;m++)
                            for(int n=0;n<data->Y_inform.sRateH;n++)
                                for(int l=0;l<8;l++)
                                    for(int k=0;k<8;k++)
                                        data->Y[data->Y_inform.sRateV*i+m][data->Y_inform.sRateH*j+n].v[l][k] *= data->qTables[data->Y_inform.DQT_ID].v[l][k];
                                    
                        //Parse all Cb
                        for (int m=0;m<data->Cb_inform.sRateV;m++)
                            for(int n=0;n<data->Cb_inform.sRateH;n++)
                                for(int l=0;l<8;l++)
                                    for(int k=0;k<8;k++)
                                         data->Cb[data->Cb_inform.sRateV*i+m][data->Cb_inform.sRateH*j+n].v[l][k] *= data->qTables[data->Cb_inform.DQT_ID].v[l][k];
                                 
                        //Parse all Cr
                        for (int m=0;m<data->Cr_inform.sRateV;m++)
                            for(int n=0;n<data->Cr_inform.sRateH;n++)
                                for(int l=0;l<8;l++)
                                    for(int k=0;k<8;k++)
                                         data->Cr[data->Cr_inform.sRateV*i+m][data->Cr_inform.sRateH*j+n].v[l][k] *= data->qTables[data->Cr_inform.DQT_ID].v[l][k];
                }
            }

        }
};

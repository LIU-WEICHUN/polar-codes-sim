#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#include "channel_coding.h"
#include <unistd.h>
#include <assert.h>
#include "polar.h"
#include "comm_smi.h"
int L = 4;
int message_size = 5;

struct path
{
    int* code;
    double PM;
};
void recursive_caluelation(int code_size, double channel_llr[][L], int* decoded_code, double* decode_llr, int L, struct path* path_[2*L], int* path_len, int channel_index);
void SCL_decoder(int code_size, int* decoded_code,double* channel_llr, int* frozen_index,int frozen_size, int* frozen_value, double* decode_llr);

int main(int argc, char const *argv[])
{   
    int code_size = atoi(argv[1]);
    int encode[code_size];
    int code[code_size];
    int decoded[code_size];
    double channel_llr[code_size];
    int frozen_size;
    int frozen_value[code_size];
    int sample_snr = 100;
    int sample_llr = 10000;
    // for sample
    sample_snr = 2;
    sample_llr = 2;

    double x_dB[sample_snr];
    double x[sample_snr];
    double decode_llr[code_size];
    double entropy_decode[code_size];
    double entropy_channel[code_size];
    double capacity_channel[code_size];
    double capacity_decode[code_size];
    FILE *f = fopen("polar_snr0715.py", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    gsl_rng * r;
    const gsl_rng_type * T;
  /* create a generator chosen by the
     environment variable GSL_RNG_TYPE */
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
    gsl_rng_set(r, time(0));

    
    // set code word
    // 1 2
    // 1 2 2 4
    // 1 2 2 4 2 4 4 8
    // 1 2 2 4 2 4 4 8 2 4 4 8 4 8 8 16
    int message[message_size];
    int ips_1024_25[] = {1023,1022,1021,1019,1015,1007, 991, 959, 895, 767, 894, 958, 990,1006,1014, 511, 893, 957, 989,1005,1013, 766, 891, 955, 987,1003, 887, 765, 951, 983,1011, 943, 999, 975, 763, 879, 927,1020,1018,1017, 974, 510, 950, 759, 998, 982, 863, 892, 509, 942, 973, 949, 997, 886, 981, 941, 751, 995, 507, 890, 971, 831, 926, 764, 947, 885, 878, 939, 925, 503, 762,1016, 735, 979,1004, 883, 877, 935, 761, 923, 967, 956, 758, 862, 495, 875, 988,1012, 757, 919, 703, 861, 508, 889, 954,1002, 985, 871, 750, 755, 953,1010, 859, 506, 911, 479, 986, 749, 830,1001, 505,1009, 639, 966, 734, 829, 934, 747, 502, 924, 855, 965, 733, 940, 876, 922, 827, 501, 888, 447, 743, 760, 933, 963, 847, 731, 874, 918, 494, 984, 499, 702, 756, 823, 952, 917, 493, 860, 701, 870,1008, 754, 727, 383, 910, 858, 869, 748, 931, 478, 815, 972, 491, 699, 915, 884, 909, 857, 477, 746, 719, 994, 854, 638, 828, 948, 504, 487, 980, 695, 907, 745, 853, 637, 826, 475, 799, 732, 742, 867, 446, 500, 938, 846, 255, 825, 851, 753, 730, 970, 741, 903, 635, 445, 687, 822, 845, 882, 498, 471, 729,1000, 946, 821, 492, 739, 726, 443, 700, 921, 497, 843, 631, 490, 873, 814, 725, 977, 463, 382, 819, 698, 671, 937, 881, 839, 813, 439, 489, 381, 718, 476, 993, 945, 697, 486, 723, 623, 969, 978, 694, 996, 474, 798, 717, 485, 811, 379, 908, 431, 636, 856, 473, 693, 797, 906, 920, 715, 483, 254, 470, 634, 607, 807, 444, 902, 824, 375, 844, 686, 752, 795, 691, 469, 253, 880, 633, 442, 415, 852, 842, 711, 685, 820, 630, 728, 462, 367, 944, 467, 251, 441, 901, 791, 744, 629, 818, 575, 683, 670, 968, 496, 724, 461, 438, 838, 812, 872, 380, 936, 669, 622, 437, 627, 722, 837, 810, 488, 783, 247, 459, 916, 679, 378, 351, 696, 621, 716, 817, 667, 964, 430, 435, 484, 377, 740, 796, 714, 692, 806, 835, 606, 619, 455, 429, 374, 472, 239, 482, 794, 713, 805, 690, 663, 899, 605, 373, 710, 319, 684, 414, 468, 868, 427, 793, 615, 252, 689, 481, 632, 962, 790, 709, 603, 413, 682, 366, 803, 655, 466, 371, 574, 250, 223, 930, 628, 423, 789, 440, 460, 681, 365, 465, 573, 850, 668, 411, 249, 678, 707, 626, 599, 914, 782, 436, 458, 246, 787, 350, 666, 363, 677, 620, 866, 961, 571, 625, 781, 457, 245, 738, 407, 434, 665, 191, 349, 809, 618, 591, 454, 675, 428, 662, 841, 359, 929, 433, 376, 238, 779, 567, 617, 243, 721, 661, 453, 347, 604, 426, 614, 399, 237, 905, 318, 849, 372, 737, 654, 602, 425, 613, 659, 775, 127, 559, 317, 976, 451, 222, 235, 412, 865, 422, 913, 653, 601, 343, 370, 932, 611, 221, 598, 410, 421, 572, 364, 992, 792, 315, 688, 369, 231, 651, 788, 597, 248, 816, 409, 543, 570, 362, 335, 219, 419, 406, 480, 190, 590, 680, 595, 311, 569, 361, 736, 244, 647, 864, 405, 589, 189, 780, 348, 566, 358, 624, 664, 808, 215, 464, 242, 398, 565, 357, 346, 403, 912, 303, 587, 778, 187, 236, 660, 616, 241, 397, 720, 345, 432, 558, 563, 777, 355, 342, 207, 234, 676, 658, 126, 316, 612, 583, 557, 774, 395, 341, 183, 424, 652, 233, 600, 125, 848, 287, 456, 904, 314, 610, 220, 230, 773, 555, 334, 542, 650, 339, 420, 596, 313, 712, 391, 218, 229, 452, 123, 175, 368, 333, 408, 310, 541, 649, 609, 418, 594, 217, 840, 551, 309, 646, 227, 568, 214, 588, 404, 360, 331, 188, 539, 900, 593, 119, 804, 213, 302, 645, 307, 586, 564, 402, 356, 159, 186, 327, 301, 396, 344, 771, 206, 585, 111, 535, 562, 401, 211, 354, 185, 708, 240, 898, 582, 556, 394, 205, 182, 299, 286, 340, 561, 353, 124, 643, 450, 181, 285, 554, 393, 527, 232, 338, 203, 122, 834,  95, 295, 390, 312, 332, 553, 174, 337, 283, 179, 228, 581, 121, 540, 550, 786, 674, 330, 308, 199, 173, 118, 216, 389, 226, 538, 549, 579, 897, 329, 306, 279, 117,  63, 158, 706, 171, 212, 387, 326, 537, 300, 225, 801, 305, 157, 547, 110, 534, 210, 115, 325, 657, 298, 833, 271, 167, 184, 204, 109, 417, 673, 533, 209, 155, 297, 323, 284, 202, 180, 294, 928, 449,  94, 107, 526, 705, 531, 282, 201, 293, 178, 151, 785,  93, 802, 525, 198, 836, 172, 960, 560, 103, 281, 177, 120, 291, 278, 197,  91, 170, 352, 143, 552,  62, 523, 608, 277, 116, 336, 169,  61, 195, 156, 166, 448,  87, 114, 270, 704, 275, 548, 165, 154, 519, 592,  59, 108, 113, 536, 269, 224, 304, 153, 800,  79, 106, 416, 163, 150, 328,  55, 532, 267, 105, 296, 208, 149, 672,  92, 584, 102, 530, 292, 142,  90, 101, 200, 147, 784,  47, 263, 524, 280, 176, 141, 400,  89, 290, 196,  99,  86, 656, 522,  60, 276, 168, 139,  85, 776, 194,  31,  58, 521, 274, 164,  78, 518,  83, 324,  57, 392, 135, 268, 112, 152,  77,  54, 193, 162, 517, 266,  53, 148, 104,  75, 772, 648, 265,  46, 161, 146,  51, 100, 262, 580,  71,  45, 140,  88, 388,  98, 145, 261, 138,  30,  84,  43,  97, 515, 769,  29,  82, 137, 259, 134,  39,  56,  76,  81,  27, 641, 133, 322,  52,  74, 546,  73,  50,  23, 770, 386, 131,  70,  44,  49, 577,  69, 273,  42,  15, 642,  41, 289, 832,  28,  67, 321,  38, 529,  26,  37, 385, 545,  25, 578, 644,  35,  22, 896,  96,  21,  80,  48,  14, 192,  19, 384,  13,  40, 160,  11,  36,  72,  24,   7, 544, 320,  20,  34,  18, 576,  12, 144,  10, 288,   9,   6, 528,   5, 272, 520, 136,  68, 640, 264, 132, 516,   3, 260, 514,  66, 130, 258, 513,  17,  33,  65, 129, 257, 768, 256, 128,  64, 512,  32,  16,   8,   4,   2,   1,   0};
    int test_16[] = {15,14,13,11,7,12,10,9,6,5,3,8,4,2,1,0};
    int* unfrozen_index = test_16;
    setZeroCodeword(frozen_value, code_size);
    setAllCodewordTo(1, message, message_size);
    setMessage2Code(message, message_size, code, code_size, unfrozen_index, frozen_value);
    PolarEncoder(code, code_size, encode);
    PrintCode(code, code_size);
    PrintCode(encode, code_size);


    // sample
    linspace(x_dB, -20, 10, sample_snr);
    dB2Linear(x_dB, x, sample_snr);
    fprintPyArray(f, "SNRvec", x, sample_snr);
    
    for (int i = 0; i < sample_snr; i++)
    {
        printf("\n\n## snr = %lf", x[i]);
        for (int j = 0; j < sample_llr; j++)
        {
            Code2LLRWithSNR(encode, channel_llr, code_size, x[i], r);
            SCL_decoder(code_size, decoded, channel_llr, unfrozen_index, frozen_size, frozen_value, decode_llr);
        }
        fprintf(f, "# sample number is: %d\n", sample_llr);
        fprintf(f, "# snr is: %lf\n", x[i]);
        fprintf(f, "a%d", i);
    }
    fclose(f);
    
    return 0;
}


double newPM(double pm_old, double llr, int code){
    if (code == 1)
    {
        return pm_old + log(1+exp(llr));
    }else if (code == 0)
    {
        return pm_old + log(1+exp(-llr));
    }else
    {
        printf("some code wrong");
        return NAN;
    }  
}

void minSorter(struct path** path_0, struct path** path_1){
    if ((*path_0)->PM <= (*path_1)->PM)
    {   
        return;
    }else
    {
        struct path* tmp = (*path_0);
        (*path_0) = (*path_1);
        (*path_1) = tmp;
    }
    return;
}

void sortPM(struct path** path_, int path_len ,int L){
    int tmp[L];
    struct path* tmp2;
    int ptr1 = 0;
    int ptr2 = L;
    int ptr1_size = path_len;
    int ptr2_size = path_len;
    int less_index;

    for (int i = 0; i < L; i++)
    {
        less_index = i;
        for (int j = i+1; j < path_len; j++)
        {
            if ((path_[j]->PM) < (path_[less_index]->PM) )
            {
                less_index = j;
            }
        }
        tmp2 = path_[i];
        path_[i] = path_[less_index];
        path_[less_index] = tmp2;
    }
    return;
       
}

void recursive_caluelation(int code_size, double channel_llr[][L], int* decoded_code, double* decode_llr, int L, struct path* path_[2*L], int* path_len, int channel_index){
    int sub_channel_size = code_size/2;
    printf("\n>>>>>>call code %d:\n", channel_index);
    printf("------\n");
    for (int i = 0; i < *path_len; i++)
    {
        printf("*path%d:\n", i);
        printf("%lf\n", path_[i]->PM);
        PrintCode((path_[i]->code)+channel_index, code_size);
    }
    printf("------\n");

    if(code_size == 1)
    {
        printf("[code size = 1]\n");
        if (decoded_code[0] == -1) //unfrozen bit
        {   
            // printf("unfrozen bits\n"); 
            // printf("when llr = %lf\n", channel_llr[0][0]);
            // printf("0PM: %lf\n", newPM(path_[0]->PM, channel_llr[0][0], 0));
            // printf("1PM: %lf\n", newPM(path_[0]->PM, channel_llr[0][0], 1));

            for (int i = 0; i < *path_len; i++)
            {
                // some index method may make it more efficiente
                //duplicate
                for (int j = 0; j < channel_index; j++)
                {
                    path_[i+(*path_len)]->code[j] = path_[i]->code[j];
                }

                path_[i+(*path_len)]->code[channel_index] = 1;
                path_[i+(*path_len)]->PM = newPM(path_[i]->PM, channel_llr[0][i], 1);

                path_[i]->code[channel_index] = 0;
                path_[i]->PM = newPM(path_[i]->PM, channel_llr[0][i], 0);
            }
            *path_len = *path_len*2;

            sortPM(path_, *path_len, L);
            if (*path_len > L)
            {
                *path_len = L;
            }

            // PM become same, something wrong
            
            printf("------\n");
            for (int i = 0; i < *path_len; i++)
            {
                printf("*path%d:\n", i);
                printf("%lf\n", path_[i]->PM);
                PrintCode((path_[i]->code)+channel_index, code_size);
            }
            printf("------\n");

            printf("return==============\n\n");
            return;
        }else   //frozen bits
        {
            // is frozen shoudn't add PM ?
            // answer: with different llr, the frozen PM is different
            for (int i = 0; i < *path_len; i++)
            {
                path_[i]->code[channel_index] = decoded_code[0];
                path_[i]->PM = newPM(path_[i]->PM, channel_llr[0][i], decoded_code[0]);
            }
            // same as upper
            // decode_llr[0] = channel_llr[0];
            printf("------\n");
            for (int i = 0; i < *path_len; i++)
            {
                printf("*path%d:\n", i);
                printf("%lf\n", path_[i]->PM);
                PrintCode((path_[i]->code)+channel_index, code_size);
            }

            //sorter
            sortPM(path_, *path_len, L);

            printf("------\n");

            printf("return==============\n\n");
            return;
        }
    }
    if(code_size%2 != 0){
        printf("the size is : %d\n", code_size);
        printf("code size unavaliable!!\n");
    }

    double llr_subchannel[sub_channel_size][L];
    
    for (int i = 0; i < sub_channel_size; i++)
    {
        for (int j = 0; j < *path_len; j++)
        {            
            llr_subchannel[i][j] = LLRUpperMaxStar(channel_llr[i][j], channel_llr[i+sub_channel_size][j]);
        } 
    }
    recursive_caluelation(sub_channel_size, llr_subchannel, decoded_code, decode_llr, L, path_, path_len, channel_index);
    //will get two PM
    // here encode assume put diffent bit 
    for (int i = 0; i < sub_channel_size; i++)
    {
        // [Done]encode assume should chanege whit case path_
        for (int j = 0; j < *path_len; j++)
        {
            llr_subchannel[i][j] = LLRLower(channel_llr[i][j], channel_llr[i+sub_channel_size][j], path_[j]->code[channel_index+i]);
        }       
        // [Done]how to solve the different llr
    }
    
    recursive_caluelation(sub_channel_size, llr_subchannel, decoded_code+sub_channel_size, decode_llr+sub_channel_size, L, path_, path_len, channel_index+sub_channel_size);
    
    for (int j = 0; j < *path_len; j++)
    {
        for (int i = 0; i < sub_channel_size; i++)
        {   
            // // test for bit add
            // if(path_[j]->code[i+channel_index] != 0 && path_[j]->code[i+channel_index] != 1){
            //     printf("!!!error bit in code[i+channel_index] = %d\n", path_[j]->code[i+channel_index]);
            //     printf("path = %d, channel index = %d, i = %d\n", j, channel_index, i);
            // }
            // if(path_[j]->code[i+channel_index+sub_channel_size] != 0 && path_[j]->code[i+channel_index+sub_channel_size] != 1){
            //     printf("!!!error bit in code[i+channel_index+sub_channel_size] = %d\n", path_[j]->code[i+channel_index+sub_channel_size]);
            //     printf("path = %d, channel index = %d, i = %d, sub_channel_size = %d\n", j, channel_index, i, sub_channel_size);
            // }
            path_[j]->code[i+channel_index] = (path_[j]->code[i+channel_index] + path_[j]->code[i+channel_index+sub_channel_size])%2;
        }    
    }
    
    for (int i = 0; i < *path_len; i++)
    {
        printf("*path%d:\n", i);
        printf("%lf\n", path_[i]->PM);
        PrintCode((path_[i]->code)+channel_index, code_size);
    }
    printf("return==============\n\n");
}

void SCL_decoder(int code_size, int* decoded_code,double* channel_llr, int* unfrozen_index,int frozen_size, int* frozen_value, double* decode_llr){
    struct path* path_[2*L];
    struct path space[2*L];
    int init_len = 1;
    int* path_len = &init_len;
    int channel_index = 0;
    int code_each_path[2*L][code_size];
    
    //set frozen bits to decoder
    SetFrozenBits2Code(decoded_code, code_size, unfrozen_index, message_size, frozen_value);
    PrintCode(decoded_code, code_size);
    printf("start decode:\n");
    
    for (int i = 0; i < 2*L; i++)
    {
        path_[i] = space+i;
        space[i].code = code_each_path[i];
        space[i].PM = 0;
    }
    
    double sub_channel_llr[code_size][L];
    for (int i = 0; i < code_size; i++)
    {
        sub_channel_llr[i][0] = channel_llr[i];
    }

    recursive_caluelation(code_size, sub_channel_llr, decoded_code, decode_llr, L, path_, path_len, channel_index);
    
    for (int i = 0; i < code_size; i++)
    {
        decoded_code[i] = path_[0]->code[i];
    }
    printf("!!!end decode\n");
    for (int i = 0; i < *path_len; i++)
    {
        printf("### path %d is:\n", i);
        printf("PM is : %lf\n", path_[i]->PM);
        PrintCode(path_[i]->code, code_size);
    }
    
    return;
}


// $Id$
#include "vlc_convolutional_coding.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

vlc_convolutional_coding::vlc_convolutional_coding(int _N, int _K, int *_poly,int _length, int _data_rate):
	N(_N),K(_K),length(_length), data_rate(_data_rate)
{
	poly = new int[3];
	memcpy(poly, _poly, sizeof(int)*3);
	no_states= powf(2, (K-1));
	//printf("K:%d\n", K);
	int a = powf(2,K);
	xor_table = new int[a];
	if (N!=3)
	{
		printf("The N value must be 3, because is the code generator value\n");
		exit(-1);
	}
	output_reverse_int = new int[2*no_states];
	memset(output_reverse_int,0,sizeof(int)*2*no_states);
	memset(xor_table,0, sizeof(int)*a);
	//printf("El valor de powf(2,K):%d\n", a);
	//mother convolutional code n=3! important
	set_generator_polynomials(xor_table,poly,K,N,no_states, output_reverse_int);
	
	//m=K-1;
	//printf("El valor de N:%d, el valor de K:%d, el valor de poly[0]:%d, poly[1]:%d, poly[2]:%d, el valor de length:%d, el valor de data_rate:%d\n", N,K, poly[0], poly[1], poly[2], length, data_rate);
	encoder_state = 0;
	punct_matrix= new int[N*2];
	ones=0;
	switch (data_rate)
	{
		case 0: //1/2, then we have to transform to 1/4
			punct_matrix[0]=1; punct_matrix[1]=1;
			punct_matrix[2]=1; punct_matrix[3]=1;
			punct_matrix[4]=0; punct_matrix[5]=0;
			ones =4;
			break;
		case 1: //1/3
			punct_matrix[0]=1; punct_matrix[1]=1;
			punct_matrix[2]=1; punct_matrix[3]=1;
			punct_matrix[4]=1; punct_matrix[5]=1;
			ones = 6;
			break;
		case 2: //2/3
			punct_matrix[0]=1; punct_matrix[1]=0;
			punct_matrix[2]=1; punct_matrix[3]=1;
			punct_matrix[4]=0; punct_matrix[5]=0;
			ones =3;
			break;
		default:
			printf("Convolutional coding configuration not suitable\n");
			exit(-1);
			break;
	}
}

vlc_convolutional_coding::~vlc_convolutional_coding()
{
	if (xor_table && punct_matrix && output_reverse_int)
	{
		delete [] xor_table;
		delete [] punct_matrix;
		delete [] output_reverse_int;
		xor_table =0;
		punct_matrix =0;
		output_reverse_int=0;
	}
}

//ENCODING ROUTINES

void vlc_convolutional_coding::set_generator_polynomials(int *xxor, int *poly, int K, int N, int no_states, int *output_reverse_int)
{
   // Generate table look-up of weight of integers of size K bits
   int i;
   int limit;
   int zero_one_output[2];
   limit=(int) pow((double)2,(double)K);
   //printf("El valor de no_states:%d, limit:%d\n", no_states, limit);
   //limit=128;
   //no_states= 64; //2^(K-1)
    
   for (i = 0; i < limit; i++) 
   {
      xxor[i] = (weight_int(K, i) & 1);
   }
    
    
    for (i=0; i<no_states;i++)
    {
      output_reverse(N, i, poly, xxor, zero_one_output);
      output_reverse_int[i]= zero_one_output[0];
      output_reverse_int[no_states+i]= zero_one_output[1];      
    }
}

int vlc_convolutional_coding::weight_int(int length, int in)
{
  int i, w = 0;
  for (i = 0; i < length; i++) {
    w += (in & (1 << i)) >> i;
  }
  return w;
}

void vlc_convolutional_coding::output_reverse(int N, int state, int *poly, int *xor_table, int *zero_one_output)
{
  int j, temp, temp_state;
  //bin one_bit;
  int one_bit; //la xor_int_table is either a vector of integers

  zero_one_output[0] = 0;
  zero_one_output[1] = 0;
  //in position one, we store the value for the one, and in position zero, we store the value for the zero
  temp_state = (state << 1) | 1;
  for (j = 0; j < N; j++) 
  {
    temp = temp_state & poly[j];
    one_bit = temp & 1;
    temp = temp >> 1;

    zero_one_output[1] = (zero_one_output[1] << 1) | (xor_table[temp] ^ one_bit);
    zero_one_output[0] = (zero_one_output[0] << 1) | (xor_table[temp]);
  }
}

void vlc_convolutional_coding::encode( int *data,  int *output,int *xor_table, int *poly, int encoder_state, int N, int K,int NN)
{
  int i,j;
  int temp;
  int M = K-1;
  
  for (i = 0; i < NN; i++) 
  {
    encoder_state |=  data[i] << M;
    for (j = 0; j < N; j++) 
    {
      temp = encoder_state & poly[j];
      output[i * N + j] = xor_table[temp];
    }
    encoder_state >>= 1;
  }
  
  // add tail of m = K-1 zeros
  for ( i = NN; i < NN + M; i++) 
  {
    for ( j = 0; j < N; j++) 
    {
      temp = encoder_state & poly[j];
      output[i * N + j] = xor_table[temp];
    }
    encoder_state >>= 1;
  }
}

int vlc_convolutional_coding::encode_punct(int *data, int *output, int *xor_table, int *poly, int encoder_state, int N, int K,int NN, int Period, int *punct_matrix)
{
	//N has to be always 3!! (according to the standard)
  int i,j,k=0,p=0;
  int length= (NN+(K-1))*N;
  //printf("El valor de Period:%d\n", Period);
  encode(data,output,xor_table, poly, encoder_state, N, K, NN);
  
  for (i =0; i<(length/N); i++)
  {
    for (j=0; j<N; j++)
    {
      if (punct_matrix[j*Period +p] == 1)
      {
			output[k]= output[i * N + j];
			k++;
      }
  	}
    p = (p+1) % Period;
  }
  return k;
}

//DECODING ROUTINES

void vlc_convolutional_coding::decode(int *encoded, int *decoded, int NN, int K, int N, int no_states, int *output_reverse_int)
{
  /*NN=number of data
  n is the rate of the code, in our case 3
  */
  int m=K-1;
  int block_length= (NN+m);
  int S0[2]; //S0 and S1
  int i,l,s; //loop variables
  double temp_metric_zero, temp_metric_one;
  int dm = pow((double)2,N);
  
  //Inizialitations
  //clear visited states
  int *temp_sum_metric = new int[no_states];
  int *temp_rec=new int[N];
  int *delta_metrics= new int[dm]; //size determined by 2^no_outputs=2^n
  int *temp_visited_state = new int[no_states];
  int *path_memory = new int[no_states*block_length];
  int *visited_state = new int[no_states]; //1 means visited state, 0 means not visited state
  int *sum_metric = new int[no_states];
  
  memset(visited_state,0,sizeof(int)*no_states);
  memset(temp_visited_state,0,sizeof(int)*no_states);
  memset(path_memory,0,sizeof(int)*(no_states*block_length));
  //clear accumulated metrics
  memset(sum_metric,0,sizeof(int)*no_states);
  memset(delta_metrics,0,sizeof(int)*dm);
  
  visited_state[0]=1; //starts in the zero state
   
  
  // evolve up to m where not all states visited
  
  if (block_length - m <=0)
  {
    printf("Input sequence too short, the program will abort\n");
    exit(-1);
  }
  
  for (l = 0; l < m; l++) // all transitions including the tail
  { 
    for (i=0; i<N; i++)
      temp_rec[i]=encoded[l*N+i];
      //temp_rec = received_signal.mid(l * n, n);

    // calculate all metrics for all codewords at the same time
    calc_metric(temp_rec, delta_metrics,N);

    for (s = 0; s < no_states; s++)  //all states
    { 
      // S0 and S1 are the states that expanded end at state s
      previous_state_2(s, S0, no_states);
            
      if (visited_state[S0[0]]==1) 
      { // expand trellis if state S0 is visited
        temp_metric_zero = sum_metric[S0[0]]+ delta_metrics[output_reverse_int[s]];
        temp_visited_state[s] = 1;
      }
      else 
      {
        //temp_metric_zero = std::numeric_limits<double>::max();
	temp_metric_zero = 9999999;
      }
      
      if (visited_state[S0[1]]) 
      { // expand trellis if state S0 is visited
        temp_metric_one = sum_metric[S0[1]]+ delta_metrics[output_reverse_int[s+no_states]];
        temp_visited_state[s] = 1;
      }
      else 
      {
        //temp_metric_one = std::numeric_limits<double>::max();
	temp_metric_one = 9999999;
      }
      if (temp_metric_zero < temp_metric_one) // path zero survives
      { 
        temp_sum_metric[s] = temp_metric_zero;
        path_memory[s*block_length+l] = 0;
      }
      else // path one survives 
      { 
        temp_sum_metric[s] = temp_metric_one;
        path_memory[s*block_length+l] = 1;
      }
    } // all states, s
    for (i=0;i<no_states; i++)
    {
      sum_metric[i] = temp_sum_metric[i];
      visited_state[i] = temp_visited_state[i];
    }
  } // all transitions, l
  // evolve from m and to the end of the block
  for (l = m; l < block_length; l++) // all transitions except the tail
  { 
    for (i=0; i<N; i++)
      temp_rec[i]=encoded[l*N+i];
      //temp_rec = received_signal.mid(l * n, n);

    // calculate all metrics for all codewords at the same time
    calc_metric(temp_rec, delta_metrics, N);

    for (s = 0; s < no_states; s++) 
    { // all states
      // S0 and S1 are the states that expanded end at state s
      previous_state_2(s, S0, no_states);
      temp_metric_zero = sum_metric[S0[0]]+ delta_metrics[output_reverse_int[s]];
      temp_metric_one = sum_metric[S0[1]] + delta_metrics[output_reverse_int[s+no_states]];
      if (temp_metric_zero < temp_metric_one) // path zero survives
      { 
        temp_sum_metric[s] = temp_metric_zero;
        path_memory[s*block_length+l] = 0;
      }
      else // path one survives 
      { 
        temp_sum_metric[s] = temp_metric_one;
        path_memory[s*block_length+l] = 1;
      }
    } // all states, s
    for (i=0; i<no_states; i++)
      sum_metric[i] = temp_sum_metric[i];
  } // all transitions, l

  // minimum metric is the zeroth state due to the tail
  int min_metric_state = 0;
  // trace back to remove tail of zeros
  for (l = block_length - 1; l > block_length - 1 - m; l--) 
  {
    // previous state calculation
    min_metric_state = previous_state_1(min_metric_state,path_memory[min_metric_state*block_length+l],m);
  }

  // trace back to calculate output sequence
  for (l = block_length - 1 - m; l >= 0; l--) 
  {
    decoded[l] = get_input(min_metric_state,m);
    // previous state calculation
    min_metric_state = previous_state_1(min_metric_state,path_memory[min_metric_state*block_length+l],m);
  }
  delete [] temp_rec;
  delete [] delta_metrics;
  delete [] temp_sum_metric;
  delete [] temp_visited_state;
  delete [] path_memory;
  delete [] visited_state;
  delete [] sum_metric;
}

void vlc_convolutional_coding::decode_punct(int *encoded, int *decoded, int NN, int K, int N, int no_states, int *output_reverse_int, int k_size, int total, int Period, int *punct_matrix)
{
  int k = 0, i = 0, p = k_size / total, j;
  int temp_size = p * Period * N;
  // number of punctured bits in a fraction of the puncture matrix
  // correcponding to the end of the received_signal vector
  p = k_size - p * total;
  // precise calculation of the number of unpunctured bits
  // (in the above fraction of the puncture matrix)
  while (p > 0) 
  {
    for (j = 0; j < N; j++) 
    {
      if (punct_matrix[j *Period + k]== 1)
        p--;
    }
    k++;
  }
  temp_size += N * k;
  if (p != 0) 
  {
    printf("Improper length of the received punctured block, dummy bits have been added");
    exit(-1);
  }

  int *temp=new int[temp_size];

  k = 0;
  j = 0;
  p = 0;

  while (k < temp_size) 
  {
    if ((punct_matrix[j * Period + p] == 1) && (i < temp_size)) 
    {
      temp[k] = encoded[i];
      i++;
    }
    else 
    { // insert dummy symbols with same contribution for 0 and 1
      temp[k] = 0;
    }

    k++;
    j++;

    if (j == N) 
    {
      j = 0;
      p = (p + 1) % Period;
    }
  } // while

  decode(temp, decoded, NN, K, N, no_states, output_reverse_int);
  delete [] temp;
}

void vlc_convolutional_coding::calc_metric(int *rx_codeword, int *delta_metrics,int N)
{
  int no_outputs= (int)pow((double)2,N); 
  int no_loop = (int)pow((double)2,N-1);  
  int mask = no_outputs - 1;
  int temp,i,j;
  
   for (i = 0; i < no_loop; i++) 
    { // all input possibilities
      delta_metrics[i] = 0;
      temp = i;
      for (j = N - 1; j >= 0; j--) 
      {
        if (temp & 1)
          delta_metrics[i] += rx_codeword[j];
        else
          delta_metrics[i] -= rx_codeword[j];
        temp >>= 1;
      }
      delta_metrics[i ^ mask] = -delta_metrics[i]; // the inverse codeword
    }
}

//! The previous state from state given the input
int vlc_convolutional_coding::previous_state_1(int state, int input, int m) 
{
    return (((state << 1) | input) & ((1 << m) - 1));
}
//! The previous state from state given the input
void vlc_convolutional_coding::previous_state_2(int state, int *S0, int no_states) 
{
    S0[0] = (state << 1) & (no_states - 1);
    S0[1] = S0[0] | 1;
}

int vlc_convolutional_coding::get_input(int state,int m) 
{ 
  return (state >> (m - 1)); 
}

int vlc_convolutional_coding::min_index(int *sum_metric, int no_states)
{
  int i, index=0, threshold=999999;
  for (i=0; i<no_states; i++)
  {
    if (sum_metric[i]<threshold)
    {
      index=i;
      threshold=sum_metric[i];
    }
  }
  return index;
}

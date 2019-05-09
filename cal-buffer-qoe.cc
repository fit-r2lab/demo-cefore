#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>

#define TIME_INTERVAL 1.0
//#define HEADER 7
#define HEADER 6 
#define VIDEO_RATE 1 // [Mbps]
#define MSS 1460 // [Byte]
//#define EXP_TIME 1800 // [sec]
#define EXP_TIME 34 // [lines]
 
int main (int argc, char *argv[])
{
  std::ifstream ifs;
  std::ofstream ofs;
  std::string str;
  std::string trash, end;
  std::string buffer;
  std::stringstream ss, iss;
 
  if(argc < 3){
    fprintf(stderr, "Usage: ./a.out [inputfile] [tcp or cefore] [initial buffer level (sec), default 1]\n"); 
    exit(-1); 
  }

  if(argv[1] == NULL){
    fprintf(stderr, "argv[1] inputfile!!\n");
    exit(-1);
  }
  std::string infile = argv[1];
  fprintf(stdout, "infile: %s\n", argv[1]);

  if( strcmp(argv[2],"tcp") != 0 && strcmp(argv[2],"cefore") != 0 ){
    fprintf(stderr, "argv[2] is [tcp or cefore]\n");
    exit(-1);
  }

  if( strcmp(argv[2], "tcp") == 0 ){
    double time = 0.0;
    double bytes = 0.0;
    double throughput = 0.0;
    double cwnd = 0.0;
    double rtt = 0.0;

    // Parameters /////
    // input
    double tau = TIME_INTERVAL;
    int header = HEADER;
    // output
    double sum_throughput = 0.0;
    double sum_bytes = 0.0;
    double sum_cwnd = 0.0;
    double sum_rtt = 0.0;
    int samples = 0;
    // std::map<double, double> thp;
    std::map<int, double> thp;
    std::map<int, double> RX_byte;
    // std::map<double, double> RX_byte;
    ///////////////////
    // std::cout << a << std::endl;

    double slope = 0.0; // [packet]
    double num_consume = 0;
    slope = ((double)VIDEO_RATE * 1000000 / 8) / (double)MSS; // [packet/sec]
    num_consume = slope * tau; // the number of consumed packet per time interval
    
    //ss << "./thp.txt"; 
    ss << argv[1]; 
    ifs.open (ss.str().c_str(), std::ios::out);

    ofs.open ("throughput-tcp.txt", std::ios::out);
   
    if (ifs.fail())
    {
      std::cout << "FILEIN_ERROR::" << ss.str() << std::endl;
    }

    for (int line_count = 1; getline(ifs, buffer); ++line_count)
    {
      iss.str(buffer);

      if (line_count <= header)
      {
        continue;
      }
        else
      {
        #if 0 
        std::cout << "line = " << buffer << std::endl;
        iss >> trash; // [ID]
        std::cout << "iss(1) = " << trash << std::endl;
        iss >> time; // Time
         std::cout << "iss(2) = " << time << std::endl;
        iss >> bytes; // Transfer
         std::cout << "iss(3) = " << bytes << std::endl;
        iss >> trash; // Bytes
         std::cout << "iss(4) = " << trash << std::endl;

        iss >> throughput; // Bandwidth
        ofs << (line_count - header)  << " " << throughput << "\n";
         std::cout << "iss(5) = " << throughput << std::endl;
        
        iss >> trash; // bits/sec
        std::cout << "iss(6) = " << trash << std::endl;
        iss >> trash; // Write/Err
        std::cout << "iss(7) = " << trash << std::endl;
        iss >> trash; // Rtry
        std::cout << "iss(8) = " << trash << std::endl;
        iss >> cwnd; // Cwnd [K]
        std::cout << "iss(9) = " << cwnd << std::endl;
        iss >> rtt; // RTT [us]
        std::cout << "iss(10) = " << rtt << std::endl;
        iss >> end; 
        std::cout << "iss(11) = " << end << std::endl;
        #endif
        #if 1 
        iss >> trash; // [ID]
        iss >> time; // Time
        //std::cout << "time: " << time << std::endl;
        iss >> trash; // [time]
        iss >> trash; // [sec]
        iss >> bytes; 
        //std::cout << "bytes: " << bytes << std::endl; //KB
        iss >> trash; // [KBytes]
        iss >> throughput; // Bandwidth
        ofs << (line_count - header)  << " " << throughput << "\n";
        //std::cout << "throughput: " << throughput << std::endl;
        
        bytes = throughput * 1000000 / 8 / 1000;
        //std::cout << "bytes: " << bytes << std::endl; //KB
        #endif 
      } 

      thp.insert(std::make_pair(time/tau, throughput)); // 'double time' makes ref errors
      RX_byte.insert(std::make_pair(time/tau, bytes * 1000));
      // std::cout << "time =	" << time/tau << ", bytes =	" << bytes << ", throughput = " << throughput << std::endl;

      // calculation /////
      sum_bytes += bytes;
      sum_throughput += throughput;
      sum_cwnd += cwnd;
      sum_rtt += rtt;
      ++samples;
      ////////////////////
      iss.str("");
      iss.clear(std::stringstream::goodbit);
    }
    ofs.close();

    ifs.close();
    ss.str("");
    ss.clear(std::stringstream::goodbit);
    
    //std::cout << "total thput =	" << sum_throughput << "		[Mbps],	avg thput =	" << sum_throughput / (double) samples << "	[Mbps]" << std::endl;
    //std::cout << "total bytes =	" << sum_bytes << "	[KB],	avg byte =	" << sum_bytes / (double) samples << "	[KB]" << std::endl;
    //std::cout << "total cwnd =	" << sum_cwnd << "		[K],	avg cwnd =	" << sum_cwnd / (double) samples << "	[K]" << std::endl;
    //std::cout << "total rtt =	" << sum_rtt << "	[us],	avg rtt =	" << sum_rtt / (double) samples << "	[us]" << std::endl;
    std::map <int, double> real_RX_seq;
    std::map <int, double> real_RX_seq_total;
    std::map <int, double> consumed_RX_seq;
    std::map <int, double> consumed_RX_seq_total;
    double nSeq = 0.0;

    for (int t = 0; t <= (int)EXP_TIME; ++t)
    {
      nSeq = RX_byte[t] / (double)MSS;
      if (t == 0)
      {
        real_RX_seq[t] = nSeq;
        real_RX_seq_total[t] = nSeq;
      }
      else if (t > 0)
      {
        real_RX_seq[t] = nSeq;
        real_RX_seq_total[t] = real_RX_seq_total[t-1] + nSeq;
      }
      else
      {
        //std::cout << "t = " << t << " is negative" << std::endl;
        abort();
      }
      // std::cout << nSeq << "	packets(seqs) were transmitted during	[" << t << "-" << (t+tau)
      // << "], RX bytes = " << RX_byte[t] << ", MSS = " << (double)MSS << std::endl;
    }

    for (int t = 0; t <= (int)EXP_TIME; ++t)
    {
      nSeq = RX_byte[t] / (double)MSS;
      // std::cout << t << "	nSeq = " << nSeq << "	num_consume = " << num_consume << std::endl;
      if (nSeq > num_consume)
      {
        nSeq = num_consume;
        // std::cout << "nSeq was updated" << std::endl;
      }
      else
      {
        // do nothing
      }
        
      if (t == 0)
      {
        consumed_RX_seq[t] = nSeq;
        consumed_RX_seq_total[t] = nSeq;
      }
      else if (t > 0)
      {
        consumed_RX_seq[t] = nSeq;
        consumed_RX_seq_total[t] = consumed_RX_seq_total[t-1] + nSeq;
      }
      else
      {
        //std::cout << "t = " << t << " is negative" << std::endl;
        abort();
      }
      // std::cout << nSeq << "	packets(seqs) were transmitted during	[" << t << "-" << (t+tau)
      // << "], RX bytes = " << RX_byte[t] << ", MSS = " << (double)MSS << std::endl;
    }

    // calculate buffer level
    std::map <int, double> BufferLevel_byte;
    double Bmax = 1.0;
    double VideoRate_bytes = (double)VIDEO_RATE * 1000000 / 8; // [bytes/sec]

    double cum_txed_data = 0.0; // bytes of data transmitted by video provider (camera)
    double cum_rxed_data = 0.0; // bytes of data received by video consumer (car)
    double tmp_cum_rxed_data = 0.0; // only used for check
    bool playing = false;
    bool rebuffering = false;
    int rebuffering_count = 0;

    int initBufTime = 0;

    // std::cout << "VideoRate = " << VideoRate_bytes << " [Byte]" << std::endl;
    
    // output to files
    //ofs.open ("buffer-level.txt", std::ios::out);
    
    double init_buff;
    if(argv[3] == NULL){
      init_buff = 1.0;
    }else{
      std::string init_buffstr = argv[3];
      init_buff = std::stod(init_buffstr);
    }
    char qoelogbuff[8];
    sprintf(qoelogbuff, "%.1f", init_buff);
    std::string strbufflvl(qoelogbuff, 3); 

    for (int t = 0; t <= (int)EXP_TIME; ++t)
    {
      cum_txed_data += VideoRate_bytes;
      tmp_cum_rxed_data = cum_rxed_data + RX_byte[t];

      if(( init_buff * VideoRate_bytes) <= tmp_cum_rxed_data && (initBufTime == 0))
      {
        //std::cout << "initial_bufferTime: " << t << std::endl;
        initBufTime = t; 
      }

      if (t == 0)
      {
        // arrival of video data
        if (tmp_cum_rxed_data <= cum_txed_data)
        {
          cum_rxed_data += RX_byte[t];
          BufferLevel_byte[t] = RX_byte[t];
        }
        else // cut off the throughput
        {
          cum_rxed_data += VideoRate_bytes;
          BufferLevel_byte[t] = VideoRate_bytes;
        }
        //ofs << (double) (t * tau) << "	" << BufferLevel_byte[t] << std::endl;
      
        if (playing)
        {
          //std::cout << "palying = " << playing << " in t = " << t << std::endl;
          abort ();
        }
        else // initial state
        {
          // if (BufferLevel_byte[t] >= Bmax * VideoRate_bytes)
          if (BufferLevel_byte[t] < Bmax * VideoRate_bytes)
          {
            playing = false; // do not play yet
            // std::cout << "time = " << t << ",	now buffering" << std::endl;
            ++rebuffering_count;
          }
          else
          {
            playing = true;
            // std::cout << "time = " << t << ",	play is started" << std::endl;
          }
        }
      }
      else // t > 0
      {
        // arrival of video data
        if (tmp_cum_rxed_data <= cum_txed_data)
        {
          cum_rxed_data += RX_byte[t];
          BufferLevel_byte[t] = BufferLevel_byte[t-1] + RX_byte[t];
        }
        else // cut off the throughput
        {
          cum_rxed_data += VideoRate_bytes;
          BufferLevel_byte[t] = BufferLevel_byte[t-1] + VideoRate_bytes;
        }
        //ofs << (double) (t * tau) << "	" << BufferLevel_byte[t] << std::endl;
      
        if (playing)
        {
          // ofs << (double) (t * tau) + tau << "	" << BufferLevel_byte[t] << std::endl;
          if (BufferLevel_byte[t] >= VideoRate_bytes)
          {
            BufferLevel_byte[t] -= VideoRate_bytes; // consume bytes from the buffer
          }
          else
          {
            BufferLevel_byte[t] = 0;
            rebuffering = true;
          }
          //ofs << (double) (t * tau) + tau << "	" << BufferLevel_byte[t] << std::endl;
          
          // rebufferring check
          if (rebuffering) // rebuffering occurs
          {
            playing = false;
            ++rebuffering_count;
            // std::cout << "time = " << t << ",	play is suspended" << std::endl;
            rebuffering = false;
          }
          else
          {
            // do nothing
          }
        }
        else // initial state / suspend
        {
          if (BufferLevel_byte[t] < Bmax * VideoRate_bytes)
          {
            playing = false; // do not play yet
            // std::cout << "time = " << t << ",	now buffering" << std::endl;
            ++rebuffering_count;		  
          }
          else
          {
            playing = true;
            // std::cout << "time = " << t << ",	play is (re)started" << std::endl;
          }
        }
      }
    }
    //ofs.close();

    //ofs.open ("real-seq.txt", std::ios::out);
    //for (int t = 0; t <= (int)EXP_TIME; ++t)
    //  {
    //    ofs << (double) (t * tau) + tau << "	" << real_RX_seq[t] << std::endl;
    //  }
    //ofs.close();

    //ofs.open ("cum-real-seq.txt", std::ios::out);
    //for (int t = 0; t <= (int)EXP_TIME; ++t)
    //  {
    //    ofs << (double) (t * tau) + tau << "	" << real_RX_seq_total[t] << std::endl;
    //  }
    //ofs.close();

    //ofs.open ("consumed-seq.txt", std::ios::out);
    //for (int t = 0; t <= (int)EXP_TIME; ++t)
    //  {
    //    ofs << (double) (t * tau) + tau << "	" << consumed_RX_seq[t] << std::endl;
    //  }
    //ofs.close();

    //ofs.open ("cum-consumed-seq.txt", std::ios::out);
    //for (int t = 0; t <= (int)EXP_TIME; ++t)
    //  {
    //    ofs << (double) (t * tau) + tau << "	" << consumed_RX_seq_total[t] << std::endl;
    //  }
    //ofs.close();

    //ofs.open ("video-rate.txt", std::ios::out);
    //for (int t = 0; t <= (int)EXP_TIME; ++t)
    //  {
    //    ofs << (double) (t * tau) + tau << "	" << slope * t * tau << std::endl;
    //  }
    //ofs.close();

    //ofs.open ("loss-ratio.txt", std::ios::out);
    //for (int t = 0; t <= (int)EXP_TIME; ++t)
    //  {
    //    ofs << (double) (t * tau) + tau << "	" << 1.0 - consumed_RX_seq[t] / num_consume << std::endl;
    //  }
    //ofs.close();

    //ofs.open ("sequence.txt", std::ios::out);
    //int seq = 0;
    //for (int t = 0; t <= (int)EXP_TIME; ++t)
    //  {
    //    for (int i = 1; i <= num_consume; ++i)
    //{
    //  if (i < consumed_RX_seq[t])
    //    {
    //      ofs << (double) (t * tau) + tau << "	" << seq << std::endl;
    //    }
    //  else
    //    {
    //      break;
    //    }
        // ofs << seq << "	" << 1.0 - consumed_RX_seq[t] / num_consume << std::endl;
    //  ++seq;
    //}
    // }
    //ofs.close();
   
    #if 0 
    ofs.open ("buffer-level-byte.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        ofs << (double) (t * tau) + tau << "	" << BufferLevel_byte[t] << std::endl;
      }
    ofs.close();
    #endif

    std::string bufflvllog = "buffer-level-sec-tcp-buflvl-" + strbufflvl + ".txt";
    //ofs.open ("buffer-level-sec-tcp.txt", std::ios::out);
    ofs.open (bufflvllog, std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
    {
      ofs << (double) (t * tau) + tau << "	" << BufferLevel_byte[t] / VideoRate_bytes << std::endl;
    }
    ofs.close();

    // std::cout << "Total rebuf. count =	" << rebuffering_count << std::endl;
    //std::cout << "total rebuffering time =	" << (double)rebuffering_count * tau << " [s]" << std::endl;
   
    double QoE_MAX = 1.0 * (double)EXP_TIME;
    double QoE = (double)QoE_MAX - ((double)rebuffering_count * tau) + ((double)initBufTime-1); 
    //double QoE = (double)QoE_MAX - ((double)rebuffering_count * tau) ; 

    std::string qoelog = "qoe-tcp-buflvl-" + strbufflvl + ".txt";
    ofs.open (qoelog, std::ios::out);
    ofs << "normalized QoE: " << QoE/QoE_MAX << std::endl;
    //std::cout << "normalized QoE: " << QoE/QoE_MAX << std::endl;

    ofs.close();
  }  

/////////////////////////////////
/////////////////////////////////
 
  if( strcmp(argv[2], "cefore") == 0 ){
    double time = 0.0;
    double bytes = 0.0;
    double throughput = 0.0;
    double cwnd = 0.0;
    double rtt = 0.0;

    // Parameters /////
    // input
    double tau = TIME_INTERVAL;
    int header = HEADER;
    // output
    double sum_throughput = 0.0;
    double sum_bytes = 0.0;
    double sum_cwnd = 0.0;
    double sum_rtt = 0.0;
    int samples = 0;
    // std::map<double, double> thp;
    std::map<int, double> thp;
    std::map<int, double> RX_byte;
    // std::map<double, double> RX_byte;
    ///////////////////
    // std::cout << a << std::endl;

    double slope = 0.0; // [packet]
    double num_consume = 0;
    slope = ((double)VIDEO_RATE * 1000000 / 8) / (double)MSS; // [packet/sec]
    num_consume = slope * tau; // the number of consumed packet per time interval
    
    //ss << "./thp.txt"; 
    ss << argv[1]; 
    ifs.open (ss.str().c_str(), std::ios::out);

    ofs.open ("throughput-cefore.txt", std::ios::out);
   
    if (ifs.fail())
    {
      std::cout << "FILEIN_ERROR::" << ss.str() << std::endl;
    }

    for (int line_count = 1; getline(ifs, buffer); ++line_count)
    {
      iss.str(buffer);

      //if (line_count <= header)
      //{
      //  continue;
      //}
      //  else
      //{
        #if 0 
        std::cout << "line = " << buffer << std::endl;
        iss >> trash; // [ID]
        std::cout << "iss(1) = " << trash << std::endl;
        iss >> time; // Time
         std::cout << "iss(2) = " << time << std::endl;
        iss >> bytes; // Transfer
         std::cout << "iss(3) = " << bytes << std::endl;
        iss >> trash; // Bytes
         std::cout << "iss(4) = " << trash << std::endl;

        iss >> throughput; // Bandwidth
        ofs << (line_count - header)  << " " << throughput << "\n";
         std::cout << "iss(5) = " << throughput << std::endl;
        
        iss >> trash; // bits/sec
        std::cout << "iss(6) = " << trash << std::endl;
        iss >> trash; // Write/Err
        std::cout << "iss(7) = " << trash << std::endl;
        iss >> trash; // Rtry
        std::cout << "iss(8) = " << trash << std::endl;
        iss >> cwnd; // Cwnd [K]
        std::cout << "iss(9) = " << cwnd << std::endl;
        iss >> rtt; // RTT [us]
        std::cout << "iss(10) = " << rtt << std::endl;
        iss >> end; 
        std::cout << "iss(11) = " << end << std::endl;
        #endif
        #if 1 
        iss >> time; // Time
        //std::cout << "time: " << time << std::endl;
        iss >> throughput; // Bandwidth
        ofs << (line_count)  << " " << throughput << "\n";
        //std::cout << "throughput: " << throughput << std::endl;
        bytes = throughput * 1000000 / 8 / 1000;
        //std::cout << "bytes: " << bytes << std::endl; //KB
        #endif 
      //} 

      thp.insert(std::make_pair(time/tau, throughput)); // 'double time' makes ref errors
      RX_byte.insert(std::make_pair(time/tau, bytes * 1000));
      // std::cout << "time =	" << time/tau << ", bytes =	" << bytes << ", throughput = " << throughput << std::endl;

      // calculation /////
      sum_bytes += bytes;
      sum_throughput += throughput;
      sum_cwnd += cwnd;
      sum_rtt += rtt;
      ++samples;
      ////////////////////
      iss.str("");
      iss.clear(std::stringstream::goodbit);
    }
    ofs.close();

    ifs.close();
    ss.str("");
    ss.clear(std::stringstream::goodbit);
    
    //std::cout << "total thput =	" << sum_throughput << "		[Mbps],	avg thput =	" << sum_throughput / (double) samples << "	[Mbps]" << std::endl;
    //std::cout << "total bytes =	" << sum_bytes << "	[KB],	avg byte =	" << sum_bytes / (double) samples << "	[KB]" << std::endl;
    //std::cout << "total cwnd =	" << sum_cwnd << "		[K],	avg cwnd =	" << sum_cwnd / (double) samples << "	[K]" << std::endl;
    //std::cout << "total rtt =	" << sum_rtt << "	[us],	avg rtt =	" << sum_rtt / (double) samples << "	[us]" << std::endl;
    std::map <int, double> real_RX_seq;
    std::map <int, double> real_RX_seq_total;
    std::map <int, double> consumed_RX_seq;
    std::map <int, double> consumed_RX_seq_total;
    double nSeq = 0.0;

    for (int t = 0; t <= (int)EXP_TIME; ++t)
    {
      nSeq = RX_byte[t] / (double)MSS;
      if (t == 0)
      {
        real_RX_seq[t] = nSeq;
        real_RX_seq_total[t] = nSeq;
      }
      else if (t > 0)
      {
        real_RX_seq[t] = nSeq;
        real_RX_seq_total[t] = real_RX_seq_total[t-1] + nSeq;
      }
      else
      {
        //std::cout << "t = " << t << " is negative" << std::endl;
        abort();
      }
      // std::cout << nSeq << "	packets(seqs) were transmitted during	[" << t << "-" << (t+tau)
      // << "], RX bytes = " << RX_byte[t] << ", MSS = " << (double)MSS << std::endl;
    }

    for (int t = 0; t <= (int)EXP_TIME; ++t)
    {
      nSeq = RX_byte[t] / (double)MSS;
      // std::cout << t << "	nSeq = " << nSeq << "	num_consume = " << num_consume << std::endl;
      if (nSeq > num_consume)
      {
        nSeq = num_consume;
        // std::cout << "nSeq was updated" << std::endl;
      }
      else
      {
        // do nothing
      }
        
      if (t == 0)
      {
        consumed_RX_seq[t] = nSeq;
        consumed_RX_seq_total[t] = nSeq;
      }
      else if (t > 0)
      {
        consumed_RX_seq[t] = nSeq;
        consumed_RX_seq_total[t] = consumed_RX_seq_total[t-1] + nSeq;
      }
      else
      {
        //std::cout << "t = " << t << " is negative" << std::endl;
        abort();
      }
      // std::cout << nSeq << "	packets(seqs) were transmitted during	[" << t << "-" << (t+tau)
      // << "], RX bytes = " << RX_byte[t] << ", MSS = " << (double)MSS << std::endl;
    }

    // calculate buffer level
    std::map <int, double> BufferLevel_byte;
    double Bmax = 1.0;
    double VideoRate_bytes = (double)VIDEO_RATE * 1000000 / 8; // [bytes/sec]

    double cum_txed_data = 0.0; // bytes of data transmitted by video provider (camera)
    double cum_rxed_data = 0.0; // bytes of data received by video consumer (car)
    double tmp_cum_rxed_data = 0.0; // only used for check
    bool playing = false;
    bool rebuffering = false;
    int rebuffering_count = 0;

    int initBufTime = 0;

    // std::cout << "VideoRate = " << VideoRate_bytes << " [Byte]" << std::endl;
    
    // output to files
    //ofs.open ("buffer-level.txt", std::ios::out);
    
    double init_buff;
    if(argv[3] == NULL){
      init_buff = 1.0;
    }else{
      std::string init_buffstr = argv[3];
      init_buff = std::stod(init_buffstr);
    }
    
    char qoelogbuff[8];
    sprintf(qoelogbuff, "%.1f", init_buff);
    std::string strbufflvl(qoelogbuff, 3); 


    for (int t = 0; t <= (int)EXP_TIME; ++t)
    {
      cum_txed_data += VideoRate_bytes;
      tmp_cum_rxed_data = cum_rxed_data + RX_byte[t];

      
      if(( init_buff * VideoRate_bytes) <= tmp_cum_rxed_data && (initBufTime == 0))
      {
        //std::cout << "initial_bufferTime: " << t << std::endl;
        initBufTime = t; 
      }

      if (t == 0)
      {
        // arrival of video data
        if (tmp_cum_rxed_data <= cum_txed_data)
        {
          cum_rxed_data += RX_byte[t];
          BufferLevel_byte[t] = RX_byte[t];
        }
        else // cut off the throughput
        {
          cum_rxed_data += VideoRate_bytes;
          BufferLevel_byte[t] = VideoRate_bytes;
        }
        //ofs << (double) (t * tau) << "	" << BufferLevel_byte[t] << std::endl;
      
        if (playing)
        {
          //std::cout << "palying = " << playing << " in t = " << t << std::endl;
          abort ();
        }
        else // initial state
        {
          // if (BufferLevel_byte[t] >= Bmax * VideoRate_bytes)
          if (BufferLevel_byte[t] < Bmax * VideoRate_bytes)
          {
            playing = false; // do not play yet
            // std::cout << "time = " << t << ",	now buffering" << std::endl;
            ++rebuffering_count;
          }
          else
          {
            playing = true;
            // std::cout << "time = " << t << ",	play is started" << std::endl;
          }
        }
      }
      else // t > 0
      {
        // arrival of video data
        if (tmp_cum_rxed_data <= cum_txed_data)
        {
          cum_rxed_data += RX_byte[t];
          BufferLevel_byte[t] = BufferLevel_byte[t-1] + RX_byte[t];
        }
        else // cut off the throughput
        {
          cum_rxed_data += VideoRate_bytes;
          BufferLevel_byte[t] = BufferLevel_byte[t-1] + VideoRate_bytes;
        }
        //ofs << (double) (t * tau) << "	" << BufferLevel_byte[t] << std::endl;
      
        if (playing)
        {
          // ofs << (double) (t * tau) + tau << "	" << BufferLevel_byte[t] << std::endl;
          if (BufferLevel_byte[t] >= VideoRate_bytes)
          {
            BufferLevel_byte[t] -= VideoRate_bytes; // consume bytes from the buffer
          }
          else
          {
            BufferLevel_byte[t] = 0;
            rebuffering = true;
          }
          //ofs << (double) (t * tau) + tau << "	" << BufferLevel_byte[t] << std::endl;
          
          // rebufferring check
          if (rebuffering) // rebuffering occurs
          {
            playing = false;
            ++rebuffering_count;
            // std::cout << "time = " << t << ",	play is suspended" << std::endl;
            rebuffering = false;
          }
          else
          {
            // do nothing
          }
        }
        else // initial state / suspend
        {
          if (BufferLevel_byte[t] < Bmax * VideoRate_bytes)
          {
            playing = false; // do not play yet
            // std::cout << "time = " << t << ",	now buffering" << std::endl;
            ++rebuffering_count;		  
          }
          else
          {
            playing = true;
            // std::cout << "time = " << t << ",	play is (re)started" << std::endl;
          }
        }
      }
    }
    //ofs.close();

    #if 0
    ofs.open ("real-seq.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        ofs << (double) (t * tau) + tau << "	" << real_RX_seq[t] << std::endl;
      }
    ofs.close();

    ofs.open ("cum-real-seq.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        ofs << (double) (t * tau) + tau << "	" << real_RX_seq_total[t] << std::endl;
      }
    ofs.close();

    ofs.open ("consumed-seq.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        ofs << (double) (t * tau) + tau << "	" << consumed_RX_seq[t] << std::endl;
      }
    ofs.close();

    ofs.open ("cum-consumed-seq.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        ofs << (double) (t * tau) + tau << "	" << consumed_RX_seq_total[t] << std::endl;
      }
    ofs.close();

    ofs.open ("video-rate.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        ofs << (double) (t * tau) + tau << "	" << slope * t * tau << std::endl;
      }
    ofs.close();

    ofs.open ("loss-ratio.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        ofs << (double) (t * tau) + tau << "	" << 1.0 - consumed_RX_seq[t] / num_consume << std::endl;
      }
    ofs.close();

    ofs.open ("sequence.txt", std::ios::out);
    int seq = 0;
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        for (int i = 1; i <= num_consume; ++i)
    {
      if (i < consumed_RX_seq[t])
        {
          ofs << (double) (t * tau) + tau << "	" << seq << std::endl;
        }
      else
        {
          break;
        }
        // ofs << seq << "	" << 1.0 - consumed_RX_seq[t] / num_consume << std::endl;
      ++seq;
    }
      }
    ofs.close();
    #endif 

    #if 0 
    ofs.open ("buffer-level-byte.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
      {
        ofs << (double) (t * tau) + tau << "	" << BufferLevel_byte[t] << std::endl;
      }
    ofs.close();
    #endif

    std::string bufflvllog = "buffer-level-sec-cefore-buflvl-" + strbufflvl + ".txt";
    ofs.open (bufflvllog, std::ios::out);
    //ofs.open ("buffer-level-sec-cefore.txt", std::ios::out);
    for (int t = 0; t <= (int)EXP_TIME; ++t)
    {
      ofs << (double) (t * tau) << "	" << BufferLevel_byte[t] / VideoRate_bytes << std::endl;
    }
    ofs.close();

    // std::cout << "Total rebuf. count =	" << rebuffering_count << std::endl;
    //std::cout << "total rebuffering time =	" << (double)rebuffering_count * tau << " [s]" << std::endl;
   
    double QoE_MAX = 1.0 * (double)EXP_TIME;
    double QoE = (double)QoE_MAX - ((double)rebuffering_count * tau) + ((double)initBufTime-1); 

    std::string qoelog = "qoe-cefore-buflvl-" + strbufflvl + ".txt";
    ofs.open (qoelog, std::ios::out);
    ofs << "normalized QoE: " << QoE/QoE_MAX << std::endl;
    //std::cout << "normalized QoE: " << QoE/QoE_MAX << std::endl;

  }  
  
  return 0;

};


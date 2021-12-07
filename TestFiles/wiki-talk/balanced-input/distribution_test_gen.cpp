#include<bits/stdc++.h>

using namespace std;

int main(){
    ifstream ifs;
    string file = "wiki-talk-gl1";
    ifs.open(file.c_str());
    if(!ifs.is_open()){
        cout<<"erro1"<<endl;
        return -1;
    }

    unsigned int s, d, w, t;
    unsigned int gl = 1;//86400
    unsigned int startT = 0;//1217567876

    //edge
    // map<pair<unsigned int, unsigned int>, set<unsigned int>> ma;

    // map<pair<unsigned int, unsigned int>, unsigned int> ma2;

    //node
    map<unsigned int, set<unsigned int>> ma;
    map<unsigned int, unsigned int> ma2;

    map<unsigned int, set<unsigned int>> dma;
    map<unsigned int, unsigned int> dma2;
    while(!ifs.eof()){
        ifs>>s>>d>>w>>t;
        unsigned int tt = ceil((double)(t-startT)/(double)gl);
        
        
        //edge
        // pair<unsigned int, unsigned int> p(s,d);
        // ma[p].insert(tt);
        // ma2[p]++;

        //node
        ma[s].insert(tt);
        ma2[s]++;

        dma[d].insert(tt);
        dma2[d]++;
    
    }
    ifs.close();

    
    
    ofstream ofs,ofs2,ofs3,ofs4; 
    //ofstream ofs2;
    ofs.open("wiki_node_out_test.txt");
    ofs2.open("wiki_node_out_count.txt");
    ofs3.open("wiki_node_in_test.txt");
    ofs4.open("wiki_node_in_count.txt");
    map<unsigned int, unsigned int> sourceMap;
    map<unsigned int, unsigned int> desMap;

    // ofs2.open("wiki-balanced_edge_counnt.txt");
    // ofs.open("wiki-balanced_edge_test.txt");
    // map<pair<unsigned int, unsigned int>, unsigned int> edgeMap;
    if(!ofs.is_open()){
        cout<<"erro3"<<endl;
        return -1;
    }
    if(!ofs2.is_open()){
        cout<<"erro4"<<endl;
        return -1;
    }
    if(!ofs3.is_open()){
        cout<<"erro5"<<endl;
        return -1;
    }
    if(!ofs4.is_open()){
        cout<<"erro6"<<endl;
        return -1;
    }
    ifstream ifs2;
    ifs2.open("wiki-talk-gl1");
    if(!ifs2.is_open()){
        cout<<"erro2"<<endl;
        return -1;
    }
    int falg = 1, falg2 = 1;
    while(!ifs2.eof()){
        ifs2>>s>>d>>w>>t;
        if(sourceMap.find(s) == sourceMap.end() && falg == 1){
            ofs<<s<<" "<<"0"<<" "<< *ma[s].begin()<<" "<<*ma[s].rbegin()<<endl;
            ofs2<<ma2[s]<<endl;
        }
        if(desMap.find(d) == desMap.end()&& falg2 == 1){
            ofs3<<d<<" "<<"1"<<" "<< *dma[d].begin()<<" "<<*dma[d].rbegin()<<endl;
            ofs4<<dma2[d]<<endl;
        }
        sourceMap[s]++;
        desMap[d]++;
        if(sourceMap.size() == 100000){
            falg = 0;
        }
        if(desMap.size() == 100000){
            falg2 = 0;
        }
        if(falg == 0 && falg2 ==0){
            break;
        }

        // pair<unsigned int, unsigned int> p(s,d);

        // if(edgeMap.find(p) == edgeMap.end()){
        //     ofs2<<ma2[p]<<endl;
        //     ofs<<s<<" "<<d << " "<< *ma[p].begin()<<" "<<*ma[p].rbegin()<<endl;
        // }
        // edgeMap[p]++;
        // if(edgeMap.size() == 10000000){
        //     break;
        // }
       
    }
    ifs2.close();
    ofs.close();
    ofs2.close();
    ofs3.close();
    ofs4.close();


    // int num[11] = {8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560};
    // for(int i = 0; i < 11; ++i){
    //     string s1 = "stackoverflow_random_node_frequence_out_"; 
    //     string s2 = "_sorted.txt";
    //     string testfile = s1 + to_string(num[i]) + s2;
    //     ifs2.open(testfile.c_str());
    //     if(!ifs2.is_open()){
    //         cout<<"erro2"<<endl;
    //         return -1;
    //     }
    //     while(!ifs2.eof()){
    //         ifs2>>s>>d>>w>>t;
    //         pair<unsigned int, unsigned int> p(s,d);
    //         //cout<<*ma[p].begin()<<" "<<*ma[p].rbegin()<<endl;
    //         ofs<<s<<" "<<d << " "<< *ma[s].begin()<<" "<<*ma[s].rbegin()<<endl;
    //         ofs2<<ma2[s]<<endl;
    //         //ofs<<ma[s]<<endl;
    //     }
    //     ifs2.close();
    // }
    // ofs.close();
    return 0;

}
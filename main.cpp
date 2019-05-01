#include <utility>

#include <utility>
#include <ostream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

#define BOUNDARY_LEFT       10
#define BOUNDARY_RIGHT      11
#define COMA                12
#define BOUNDARY_VEC_LEFT   14
#define BOUNDARY_VEC_RIGHT  15
#define STR                 16
#define INT                 17
#define DOUBLE              18
#define COLON               19
#define KEY                 20

class JsonObj{

public:
    int type;   //type = 0:JsonObj,  1:Int, 2:String, 3:Vec, 4:Double
    union _data{
        map<string, JsonObj> *Obj;
        int *                       Int;
        string *                    String;
        vector<JsonObj> *           Vec;
        double *                    Double;
    } data;

    //构造函数
    JsonObj(int type, void * data){
        bool _ok = true;
        if(data == nullptr){
            _ok = false;
        }
        if(type < 0 || type > 5){
            printf("type scope is out of range! type=%d\n",type);
        }
        if(!_ok){
            this->type = 2;
            this->data.String = new string("NULL");
            return ;
        }

        this->type = type;
        this->data.Int = nullptr;
        switch (type){
            case 0: this->data.Obj = (map<string, JsonObj> *)(data);break;
            case 1: this->data.Int = (int *)(data);break;
            case 2: this->data.String = (string *)(data);break;
            case 3: this->data.Vec = (vector<JsonObj> *)(data);break;
            case 4: this->data.Double = (double *)(data);break;
            default:this->data.String = (string *)new string("NULL");
        }
    }

    // 赋值构造函数
    JsonObj(const JsonObj& obj){
        this->type = obj.type;
        switch (this->type){
            case 0:
                this->data.Obj = new map<string, JsonObj>();
                for(auto & p : *obj.data.Obj){
                    this->data.Obj->insert(pair<string, JsonObj>(p.first, JsonObj(p.second)));
                }
                break;
            case 1:
                this->data.Int = new int(*obj.data.Int);
                break;
            case 2:
                this->data.String = new string(*obj.data.String);
                break;
            case 3:
                this->data.Vec = new vector<JsonObj>();
                for(JsonObj o : *obj.data.Vec){
                    this->data.Vec->emplace_back(o);
                }
                break;
            case 4:
                this->data.Double = new double(*obj.data.Double);
                break;
        }
    }

    //析构函数
    ~JsonObj(){
        switch (type){
            case 0:
                delete this->data.Obj;break;
            case 1:
                delete this->data.Int;break;
            case 2:
                delete this->data.String;break;
            case 3:
                delete this->data.Vec;break;
            case 4:
                delete this->data.Double;break;
        }
    }

    friend ostream& operator<<(ostream& out, const JsonObj& obj);
};

ostream &operator<<(ostream &out, const JsonObj &obj) {
    switch (obj.type){
        case 0:
            out<<"{"<<endl;
            for(pair<string, JsonObj> p : *obj.data.Obj){
                out<<p.first<<": "<<p.second<<endl;
            }
            out<<"}"<<endl;
            break;
        case 1:
            out<<*obj.data.Int<<endl;
            break;
        case 2:
            out<<"\""<<*obj.data.String<<"\""<<endl;
            break;
        case 3:
            out<<"[";
            for(int i = 0;i<obj.data.Vec->size();i++){
                operator<<(out,(*obj.data.Vec)[i]);
                if(i != obj.data.Vec->size()-1)
                    out<<", ";
            }
            out<<"]";
            break;
        case 4:
            out<<*obj.data.Double<<endl;
            break;

    }
}

//词法分析过程
vector<pair<int,string>> GetToken(string data){
    vector<pair<int,string>> tokenVec;
    string::iterator begin = data.begin();
    string::iterator end;

    while (*begin != '\0' && begin != data.end()){
        end = begin;

        //处理边界
        if(*end == '{'){
            tokenVec.emplace_back(pair<int, string>(BOUNDARY_LEFT,"{"));
            end = end + 1;
            begin = end;
            continue;
        }

        if(*end == '}'){
            tokenVec.emplace_back(pair<int, string>(BOUNDARY_RIGHT,"}"));
            end = end + 1;
            begin = end;
            continue;
        }

        if((*end) ==  ':'){
            tokenVec.emplace_back(pair<int,string>(COLON,":"));
            end = end + 1;
            begin = end;
            continue;
        }

        if(*end == ','){
            tokenVec.emplace_back(pair<int,string>(COMA,","));
            end = end + 1;
            begin = end;
            continue;
        }

        //截取字符串
        if(*end == '\"'){
            stringstream ss;
            ss.clear();
            end = end+1;
            while(*end != '\"'){
                ss<<*end;
                end = end + 1;
            }
            end = end + 1;
            tokenVec.emplace_back(pair<int, string>(STR, ss.str()));
            begin = end;
            continue;
        }

        //处理数字
        if((*end >= '0' && *end <='9') || *end == '.'){
            stringstream ss;
            ss.clear();
            bool isDouble = false;
            while((*end >= '0' && *end <='9') || *end == '.'){
                if(*end == '.') isDouble = true;
                ss << *end;
                end = end + 1;
            }

            if(!isDouble){      //是整数
                tokenVec.emplace_back(pair<int,string>(INT,ss.str()));
            }else{
                tokenVec.emplace_back(pair<int,string>(DOUBLE,ss.str()));
            }

            begin = end;
            continue;
        }

        if(*end == '['){
            tokenVec.emplace_back(pair<int,string>(BOUNDARY_VEC_LEFT,"["));
            end = end + 1;
            begin = end;
            continue;
        }

        if(*end == ']'){
            tokenVec.emplace_back(pair<int,string>(BOUNDARY_VEC_RIGHT,"]"));
            end = end + 1;
            begin = end;
            continue;
        }

        //截取KEY
        if(*end != '\n' && *end != ' ' && *end != '\t'){
            stringstream ss;
            ss.clear();
            while(*end != '\n' && *end != ' ' && *end != '\t' && *end != '[' && *end != ']' && *end != ':' &&
            *end != '{'){
                ss << *end;
                end = end + 1;
            }
            begin = end;
            tokenVec.emplace_back(pair<int,string>(KEY,ss.str()));
            continue;
        }

        end = end + 1;
        begin = end;
    }

    return tokenVec;
}

//语法分析过程
JsonObj Parse(vector<pair<int, string> > &tokenVec){
    JsonObj JSON(0, nullptr);
    while (!tokenVec.empty()){
        pair<int,string> token = tokenVec[0];
        if(token.first == INT){
            JSON.type = 1;
            JSON.data.Int = new int(atoi(token.second.c_str()));
            tokenVec.erase(tokenVec.begin());
            break;
        }

        if(token.first == STR){
            JSON.type = 2;
            JSON.data.String = new string(token.second);
            tokenVec.erase(tokenVec.begin());
            break;
        }

        if(token.first == DOUBLE){
            JSON.type = 4;
            JSON.data.Double = new double(atof(token.second.c_str()));
            tokenVec.erase(tokenVec.begin());
            break;
        }

        //解析数组
        if(token.first == BOUNDARY_VEC_LEFT){
            JsonObj jsonObj(0, nullptr);
            vector<JsonObj> * vec = new vector<JsonObj>();

            tokenVec.erase(tokenVec.begin());
            while(tokenVec[0].first != BOUNDARY_VEC_RIGHT){
                if(tokenVec[0].first == COMA){
                    tokenVec.erase(tokenVec.begin());
                    continue;
                }
                JsonObj jsontmp = Parse(tokenVec);
                vec->emplace_back(jsontmp);
            }
            tokenVec.erase(tokenVec.begin());
            JSON.type = 3;
            JSON.data.Vec = vec;
            break;
        }

        //解析json
        if(token.first == BOUNDARY_LEFT){
            JsonObj jsonObj(0, nullptr);
            map<string,JsonObj> * mmap = new map<string, JsonObj>();
            tokenVec.erase(tokenVec.begin());

            while(tokenVec[0].first != BOUNDARY_RIGHT){
                pair<int, string> p = tokenVec[0];

                if(p.first == COMA){
                    tokenVec.erase(tokenVec.begin());
                    continue;
                }

                string key = "NULL";
                if(p.first == KEY){
                    key = p.second;
                }

                JsonObj obj = Parse(tokenVec);
                mmap->insert(pair<string, JsonObj>(key, obj));
            }
            tokenVec.erase(tokenVec.begin());
            JSON.type = 0;
            JSON.data.Obj = mmap;
            break;
        }

        tokenVec.erase(tokenVec.begin());
    }
    cout<<JSON<<endl;
    return JSON;
}

JsonObj JSONParse(string data, int n){
    //词法分析过程
    vector<pair<int,string> > tokenVec = GetToken(std::msove(data));

    //语法分析过程
    JsonObj JSON = Parse(tokenVec);
    return JSON;
}



int main() {
    string data = R"({aaa:"nihao",bbb: 123,ccc: 123.5, ddd: [1,22],eee:{a:"ccc"}})";
    //string data = "{aaa:\"nihao\",bbb: 123}";
    //string data = R"({aaa:"nihao"})";
    JsonObj json = JSONParse(data, data.length());
    return 0;
}







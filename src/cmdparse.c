#include "cmdparse.h"

int *mapquote(char *str){
    size_t len=strlen(str)+1;
    int *map=NEW(int,len);
    
    BOOL literal=FALSE;

    for (size_t i = 0; i < len; i++) {
        map[i]=literal;
        if (str[i]=='\"' && ((i!=0)? str[i-1]!='\\':TRUE) ) {
            literal=!literal;
            if (literal==FALSE) map[i]=FALSE; 
        }
    }

    return map;
}

char *tokfind(char *str,size_t n, char *token) {

    //char *copy=NULL;
    //strpeck(str,&copy,NULL,n);

    char *position=str;

    //copy[n]='\0'; //to force the end of the string there

    //int *quote=mapquote(copy);
    int *quote=mapquote(str);

    while (TRUE) {
        position=strstr(position,token);
        if (position-str>=n) position=NULL;
        if (position==NULL) { //If didn't found anything
            free(quote);
            //free(copy);
            return NULL;
        }
        if (!quote[position-str]) { //If it found the token and is not in a string
            free(quote);
            //free(copy);
            //return str+(position-copy);
            return position;
        }
        position++; //Found the token but is part of a string
    }
    free(quote);
    //free(copy);
    return NULL; //Shouldn't get here but just in case
}

char *tokfindr(char *str, size_t n, char *token){
    char *copy=NULL;
    char *tok_copy=NULL;
    size_t tok_len=strlen(token);
    
    strpeck(str,&copy,NULL,n);
    strpeck(token,&tok_copy,NULL,tok_len);

    strrev(copy,n); strrev(tok_copy, tok_len);

    char *occ=tokfind(copy,n,tok_copy);

    free(copy);
    free(tok_copy);

    if (occ==NULL) return NULL;

    size_t cut_rev_pos=occ-copy;
    size_t cut_pos=n-cut_rev_pos-tok_len;
    

    return str+cut_pos;
}

void __filli3(int3 *a, size_t n, int3 value) {
    for (size_t i = 0; i < n; i++)
    {
        a[i][0]=value[0];
        a[i][1]=value[1];
        a[i][2]=value[2];
    }
}

int3 *mapcond(char *str) {
    size_t start=0;
    size_t len=strlen(str);

    int3 *map=NEW(int3,len+1);

    int3 current={0,0,0}; //Current balance values goes here

    while (TRUE) {    
        char * if_pos=tokfind(str+start,len,TOK_IF);
        char * then_pos=tokfind(str+start, len,TOK_THEN);
        char * else_pos=tokfind(str+start, len,TOK_ELSE);
        char * end_pos=tokfind(str+start, len,TOK_END);

        if (    if_pos==NULL &&
                then_pos==NULL &&
                else_pos==NULL &&
                end_pos==NULL)
        break; //Finish the string
        
        //Just keep the closest to the start position, nasty macro
        char *pos=MIN(MIN(DIFNULL(if_pos,str+len),DIFNULL(then_pos,str+len)),MIN(DIFNULL(else_pos,str+len),DIFNULL(end_pos,str+len)));
        size_t length=0;

        //The closest one is an if
        if ((pos)==if_pos)
        {
            length=TOK_IF_LEN; //Length of the substring searched
            __filli3(map+start,MIN(pos-str-start,len+1-start)+length,current); //Fill map with the same value from the start until the end of the substring
            current[0]++; //Open and close the correct scopes
            start=(pos+length)-str; //Move the start position of the search
        }
        //The closest one is a then
        if (pos==then_pos)
        {
            length=TOK_THEN_LEN;
            __filli3(map+start,MIN(pos-str-start,len+1-start)+length,current);
            current[0]--;
            current[1]++;
            start=(pos+length)-str;
        }
        //The closest one is an else
        if (pos==else_pos)
        {
            length=TOK_ELSE_LEN;
            __filli3(map+start,MIN(pos-str-start,len+1-start)+length,current);
            current[1]--;
            current[2]++;
            start=(pos+length)-str;
        }
        //The closest one is an end
        if (pos==end_pos)
        {
            length=TOK_END_LEN;
            __filli3(map+start,MIN(pos-str-start,len+1-start)+length,current);
            //This is for not-else expression compatibility mostly
            if (current[2]==0) current[1]--; else current[2]--;
            start=(pos+length)-str;
        } 
    }
    __filli3(map+start,(len+1)-start,current); //Fill the rest until the last position

    return map;
}

BOOL validcond(int3 *map, size_t n) {
    if (map[n-1][0]!=0 || map[n-1][1]!=0 || map[n-1][2]!=0) return FALSE; //Didn't end at 0,0,0
    for (size_t i = 0; i < n; i++) {
        if (map[i][0]<0 || map[i][1]<0 || map[i][2]<0) return FALSE; //Went negative at some time
    }
    return TRUE;
}

int rangecond(int3 *map, size_t n,range if_range,range then_range, range else_range) {
    if (!validcond(map,n)) return -1;

    BOOL has_else=FALSE;

    range _if={n,n};
    range _then={n,n};
    range _else={n,n};

    for (size_t i = 0; i < n; i++) {
        //Define condition range (if-then)
        if (map[i][0]==1 && _if[0]==n) _if[0]=i;
        //Define then range (then-else/end)
        if (map[i][1]==1 && map[i][0]==0 && _then[0]==n) _then[0]=i;
        if (map[i][1]==0 && map[i][0]==0 && _then[0]!=n && _then[1]==n) {
            _then[1]=i-TOK_END_LEN; //Minus 4 cause "end" length is 4
        }
        //Define else range (else-end)
        if (map[i][2]==1 && map[i][1]==0 && map[i][0]==0 && _else[0]==n) {_else[0]=i; has_else=TRUE;}
        if (map[i][2]==0 && map[i][1]==0 && map[i][0]==0 && _else[0]!=n && _else[1]==n) {
            _else[1]=i-TOK_END_LEN; //Minus 4 cause "end" length is 4
        }
    }

    if (_if[0]==n && _if[1]==n) {
        _else[0]=_else[1]=_then[0]=_then[1]=_if[0];
        return -1;
    }

    if_range[0]=_if[0];
    if_range[1]=_then[0]-TOK_THEN_LEN; //Set end to start of then
    then_range[0]=_then[0];
    if (has_else) {
        else_range[0]=_else[0];
        then_range[1]=_else[0]-TOK_ELSE_LEN; //Change end to start of else
        else_range[1]=_else[1];
    }

    return has_else;
}

void chaintoken(char *line, size_t n, char **index, int *type) {
    //Similar to mapcond to find first token to appear
    char *scolon_pos=tokfind(line,n,TOK_CHAIN_NORMAL);
    char *and_pos=tokfind(line,n,TOK_CHAIN_AND);
    char *or_pos=tokfind(line,n,TOK_CHAIN_OR);

    if( scolon_pos==NULL &&
        and_pos==NULL &&
        or_pos==NULL) {
            if (index!=NULL) (*index)=NULL;
            if (type!=NULL) (*type)=0;
    }

    char *pos=MIN(MIN(DIFNULL(scolon_pos,line+n),DIFNULL(and_pos,line+n)),DIFNULL(or_pos,line+n));

    //It's a semicolon
    if (pos ==scolon_pos) {
        if (type!=NULL) (*type)=CMD_CHAIN_NORMAL;
    }
    //It's an and
    if (pos==and_pos) {
        if (type!=NULL) (*type)=CMD_CHAIN_AND;
    }
    //It's an or
    if (pos==or_pos) {
        if (type!=NULL) (*type)=CMD_CHAIN_OR;
    }
    if (index!=NULL) (*index)=pos;
}

void __endlinize(char *line, size_t n) {
    int *quotes=mapquote(line);

    //Changes spaces for endlines and keep string literals untouched
    for (size_t i=0; i < n ;i++) {
        if (quotes[i]==0 && (line[i]==' ' || line[i]=='\"')) {
            line[i]='\n';
        }
    }
}

// char *__company(char *line, size_t n, char **input, char **output, char **houtput) {
//     char *copy=line;
//     //strpeck(line,&copy,NULL,n);
//
//     char *in=tokfindr(copy,n,TOK_IN);
//     if (in!=NULL)
//         for (size_t i=0; i < TOK_IN_LEN ;i++) {
//             in[i]='\n';
//         }
//     char *hout=tokfindr(copy,n,TOK_HARDOUT);
//     if (hout!=NULL)
//         for (size_t i=0; i < TOK_IN_LEN ;i++) {
//             in[i]='\n';
//         }
//     char *out=tokfindr(copy,n,TOK_OUT);
//     if (out!=NULL)
//         for (size_t i=0; i < TOK_IN_LEN ;i++) {
//             in[i]='\n';
//         }
//
//     if (in!=NULL) in+=TOK_IN_LEN;
//     if (hout!=NULL) houtput+=TOK_HARDOUT_LEN;
//     if (out!=NULL) out+=TOK_OUT_LEN;
//
//     if (hout!=NULL) out=hout;
//
//     if (in!=NULL && input!=NULL)
//     {
//         __endlinize(in,n-(in-copy));
//         strpeck(tok,input,NULL,tokfind(in,n-(in-copy),'\n'))
//     }
//     if (out!=NULL && input!=NULL)
//     {
//         __endlinize(in,n-(in-copy));
//         strpeck(tok,input,NULL,tokfind(in,n-(in-copy),'\n'))
//     }
// }

int __catch_red(char *line,char *copy, char*token, char*prev_token) {
    line+=(prev_token+strlen(prev_token))-copy;
    char *_in=tokfindr(line,token-line,TOK_IN);
    char *_hout=tokfindr(line,token-line,TOK_HARDOUT);
    char *_out=tokfindr(line,token-line,TOK_OUT);

    if(_hout!=NULL) _out=_hout;

    if (_in==NULL && _hout==NULL && _out==NULL) return -1;

    return ((_in<_out)? _out==_hout : 2);
}

cmd *parse(char *line) {
    size_t line_size=strlen(line)+1;
    char *bg=tokfindr(line,line_size,TOK_BACKGROUND);

    // CMDP(left_cmd);
    // CMDP(right_cmd);

    cmd *left_cmd,*right_cmd;

    int chain_type;
    char *chain;
    chaintoken(line,line_size,&chain,&chain_type);

    //Line is from a complex command with chain
    if (chain!=NULL) {
        char *left, *right;
        strpeck(line,&left,&right,chain-line);
        char *right_moved=right+(chainsz(chain_type));

        left_cmd=parse(left); //Make a command out of the text left side the chain token
        right_cmd=parse(right_moved); //Make a command out of the right side

        left_cmd->options^=(CMD_CHAINEDRIGHT | (bg!=NULL)? CMD_BACKGROUND : 0);
        left_cmd->chain_type=chain_type;
        left_cmd->chain=right_cmd; //Setup chain info

        free(left);
        free(right);

        return left_cmd;
    } else {
        char *pipe=tokfind(line,line_size,TOK_PIPE);

        //The command has a pipe so is complex
        if (pipe!=NULL) {
            char *left, *right;
            strpeck(line,&left,&right,pipe-line);

            char *right_moved=right+TOK_PIPE_LEN;

            left_cmd=parse(left); //Make a command out of the text left side the chain token
            right_cmd=parse(right_moved); //Make a command out of the right side

            left_cmd->options^=(CMD_PIPEDRIGHT | (bg!=NULL)? CMD_BACKGROUND : 0);
            left_cmd->pipe=right_cmd; //Setup chain info

            free(left);
            free(right);

            return left_cmd;
        } else { //Then it's a normal command... or an if
            int3 *map=mapcond(line);
            range if_range, then_range, else_range;

            int rr=rangecond(map,line_size,if_range,then_range,else_range);

            if (rr!=-1) { //It's a conditional expression
                char *ifline=strsub(line+if_range[0],if_range[1]-if_range[0]);
                char *thenline=strsub(line+then_range[0],then_range[1]-then_range[0]);
                char *elseline=NULL;

                if (rr>0) elseline=strsub(line+else_range[0],else_range[1]-else_range[0]);

                cmd *if_cmd=parse(ifline);
                cmd *then_cmd=parse(ifline);
                cmd *else_cmd=NULL;
                if (rr>0) else_cmd=parse(ifline);

                CMDP(condition);

                condition->options=CMD_CONDITIONAL | ((rr>0)? CMD_ELSECOND : 0) | ((bg!=NULL)? CMD_BACKGROUND : 0);
                condition->conditioncmd=if_cmd;
                condition->thencmd=then_cmd;
                if (rr>0) condition->elsecmd=else_cmd;

                free(ifline);
                free(thenline);
                if (rr>0) free(elseline);

                return condition;
            }

            //Simple command code. Not so simple.

            // LKLIST(primallist);
            // LKLIST(stillprimallist);
            LKLIST(arglist);

            //Copies the line for safe use with strtok
            char *copy=NULL; 
            strpeck(line,&copy,NULL,line_size);

            int *quotes=mapquote(copy);

            //Changes spaces for endlines and keep string literals untouched
            for (size_t i=0; i < line_size ;i++) {
                if (quotes[i]==0 && (copy[i]==' ' || copy[i]=='\"')) {
                    copy[i]='\n';
                }
            }

            size_t count=0; //Count of arguments

            char *in_arg=NULL;
            char *out_arg=NULL;
            BOOL out_ishard=FALSE;

            //Erase in and out redirection from copy
            char *pos=tokfind(copy,line_size,TOK_IN);
            while (pos!=NULL)
                for (size_t i=0; i < TOK_IN_LEN ;i++)
                    pos[i]='\n';
            pos=tokfind(copy,line_size,TOK_OUT);
            while (pos!=NULL)
                for (size_t i=0; i < TOK_OUT_LEN ;i++)
                    pos[i]='\n';
            pos=tokfind(copy,line_size,TOK_HARDOUT);
            while (pos!=NULL)
                for (size_t i=0; i < TOK_HARDOUT_LEN ;i++)
                    pos[i]='\n';

            //Uses endline as token to split the arguments
            char *prev=copy;
            char *token=strtok(copy,"\n");
            while (token!=NULL) {
                char *arg=NULL;
                strpeck(token,&arg,NULL,strlen(token));
                //Catch if this is a redirection argument
                int red_arg=__catch_red(line,copy,token,prev);
                if (red_arg == -1) {
                    lkappend(&arglist,arg);
                    count++;
                } else {
                    if (red_arg==0) {
                        if (out_arg!=NULL) free(out_arg);
                        out_arg=arg;
                        out_ishard=FALSE;
                    } else if (red_arg==1) {
                        if (out_arg!=NULL) free(out_arg);
                        out_arg=arg;
                        out_ishard=TRUE;
                    } else if (red_arg==2) {
                        if (in_arg!=NULL) free(in_arg);
                        in_arg=arg;
                    }
                }

                prev=token;
                token=strtok(NULL,"\n");
            }

            char **args=NEW(char*,count+1);
            
            for (size_t i = 0; i < count; i++)
            {
                char *arg=lkquit(&arglist);
                args[i]=arg;
            }
            args[count]=NULL;

            CMDP(result_cmd);

            result_cmd->argc=count;
            result_cmd->options=( 
                (bg!=NULL)? CMD_BACKGROUND : 0 ) 
                | ( (in_arg!=NULL)? CMD_EXTINPUT : 0  )
                | ( (out_arg!=NULL)? CMD_EXTOUTPUT : 0  )
                | ( (out_ishard)? CMD_HARDEXTOUTPUT : 0  );
            result_cmd->argv=args;
            result_cmd->extinput=in_arg;
            result_cmd->extoutput=out_arg;

            return result_cmd;

            // lkfree(primallist,TRUE);
            // lkfree(stillprimallist,TRUE);
            lkfree(&arglist,FALSE);

            free(map);
            free(quotes);
        }
    }
}

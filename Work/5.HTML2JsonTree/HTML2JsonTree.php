<!DOCTYPE html>
<html>
<head>
    <meta charset='utf-8'/>
    <title>JSON TreeView Test</title> 
		<script src="http://code.jquery.com/jquery-1.9.1.js"></script> 
		<script src="jstree.min.js"></script> 
		<link rel="stylesheet" href="style.min.css"/>
		<style> 			
			.caption {
				color: orange;
				text-align: center;
				background-color: #d0e4fe;
			}
			.divbox {
				background: #dddFFF;
				border: 2px solid #ccc;
				-moz-border-radius: 15px;
				border-radius: 15px;
			}
		</style> 

</head>
<body>
		 
	<h1 id=cap class=caption align=center> JSON Tree </h1>		 
	<br/>
	<form action="HTML2JsonTree.php" method="post"> 
		 URL: <input type="url" name="url"/> 
		 <input type="submit" value="OK"/>
	</form>	 
	<br/> 
	
<?php 
$MaxLengthOfTag=8;
$tags=array("style", "script", "div", "ul", "ol", "li", "a", "p", "i", "link", "title","html", "head", "body", "meta",  "h1", "form", "input",  "button", "h2", "h3", "h4", "h5", "h6", "span", "tt", "br", "hr", "abbr", "textarea",  "center", "legend", "fieldset", "address", "cite", "noscript", "tbody", "table", "tr", "td", "small", "dl", "dt", "dd", "b", "big");

////////////////////////////////////////////////////////////////////////////////
class Tag {
    public $name;
    public $attr;
    public $value;
    public $level;
    public $hasChild;

    public function  __construct($_name, $_level){
        $this->name = $_name;
        $this->attr = "";
        $this->value = "";
        $this->level = $_level;
    }
}

function readHTMLFile($htmlFile){ 
    $s="";
    $handle = fopen($htmlFile, "r");
    while (($buffer = fgets($handle, 4096)) != false) {
        $s = $s.trim($buffer); 
    }
    fclose($handle); 
    return $s;
}

function containsTag($subs){ // a little time-consuming, don't use it much
    global $tags;
    for($i=0; $i<sizeof($tags); $i++) {
				if((strpos($subs, $tags[$i] . ">")|| strpos($subs, $tags[$i] . " "))) return true;
    }
    return false;
}

function isTagText($c){
    //return ((ord($c)<=ord('z') && ord($c)>=ord('a')) || (ord($c)<=ord('9') && ord($c>)=ord('0'))); // -, : for xml
		return preg_match("/^[a-zA-Z0-9]$/", $c);
}

function isText($c){
    //return ((ord($c)<=ord('z') && ord($c)>=ord('a')) || (ord($c)<=ord('Z') && ord($c)>=ord('A')) || (ord($c)<=ord('9') && ord($c)>=ord('0')) || $c==='-' || $c===':' ); // -, : for xml
		return preg_match("/^[a-zA-Z0-9]$/", $c) ||  $c==='-' || $c===':';
}

function isValueText($c){
    return($c!='>' && $c!='<');
}

function nSpaces($n){
    $res="";
    for($i=0; $i<$n; $i++) $res .= "  ";
    return $res;
}

function nBrackets($n, $startLevel){ 
	return str_repeat("]}", $n);
}

function isAheadValue($s, $p){
    $len=strlen($s);
    $p++;
    while($p<$len && ord($s[$p]) <=32 ) $p++; 
    return ($p<$len) && isValueText($s[$p]);
}

/*
   * remove comments from $html
   */
function commentsRemove($s){
    $p=0;
    $len=strlen($s);
    $flag=true;
    $res="";
    while($p<$len){
        if($p<$len-3 && $s[$p]==='<' && $s[$p+1]==='!' && $s[$p+2]==='-' && $s[$p+3]==='-') $flag = false;
        if($p>=3 && $s[$p-3]==='-' && $s[$p-2]==='-' && $s[$p-1]==='>') $flag = true;
        if($flag) $res .= $s[$p]; // 0.07s for google.$html. 400 times faster than String!
        $p++;
    }
    return $res;
}

/*
* if we see a pair of <a> ... <a>, we change the second one to </a>
* also we check <title>...<title> and repair this case
*/
function aTitleRepair($s){
    $seen=false;
    $p=0;
    $len=strlen($s);
    while($p<$len-3){
        if($s[$p]==='<' && $s[$p+1]==='a' && ($s[$p+2]==='>' || $s[$p+2]===' ')) {
            if($seen) { $s = substr($s, 0, $p+1) . "/" . substr($s, $p+1, $len-$p-1); $len += 1; }
            $seen = !$seen;
            $p += 3;
        }
        else if($s[$p]==='<' && $s[$p+1]==='/' && $s[$p+2]==='a'  && ($s[$p+3]==='>' || $s[$p+3]===' ')) {
            $seen = false;
        }
        $p++;
    }

    $p=0; $len=strlen($s); // check "<title>"
    while($p<$len-7){
        if($s[$p]==='<' && $s[$p+1]==='t' && $s[$p+2]==='i' && $s[$p+3]==='t' && $s[$p+4]==='l' && $s[$p+5]==='e' && $s[$p+6]==='>') {
            if($seen) { $s = substr($s, 0, $p+1) . "/" . substr($s, $p+1, $len-$p-1); $len += 1; }
            $seen = !$seen;
            $p += 7;
        }
        else if($s[$p]==='<' && $s[$p+1]==='/' && $s[$p+2]==='t' && $s[$p+3]==='i' && $s[$p+4]==='t' && $s[$p+5]==='l' && $s[$p+6]==='e' && $s[$p+7]==='>') {
            $seen = false;
        }
        $p++;
    }

    return $s;
}

/*
* some $html tend to miss "/" when closing meta, title and link (also a);
* so we repair this case.
*/
function MetaLinkInputImgRepair($s){
    $someTag = array("meta ", "link ", "input ", "img ");/////$someTag =array("meta ", "<link ", "<input ", "<img ");
    $p=0;/////$p=1;
    $len = strlen($s);

    while($p<$len-7){
        if($s[$p] !='<') { $p++; continue; }
        if($p+1<$len && !($s[$p+1] ==='m'|| $s[$p+1] ==='l' || $s[$p+1] ==='i')) { $p++; continue; }

        // now we see <m, <l or <$i:
        for($i=0; $i<sizeof($someTag); $i++) {  
						if(strpos(substr($s, $p, 7), $someTag[$i])) {
                while($p<$len && $s[$p] != '>') $p++;
                if($p===$len) break;
                if($s[$p-1]!='/') $s =  substr($s, 0, $p) . "/" .  substr($s, $p, $len-$p);
                break;
            }
        }
        $len = strlen($s);
        $p++;
    }
    return $s;
}
 
/*
* if $html misses </head>, </body>, </$html>, then we add them into $html.
*/ 
function HeadBodyHtmlRepair($s) { 
		if(strpos($s, "<html>") || strpos($s, "<html ")) { 
				if(!strpos($s, "</html>")) $s .= "</html>";

        // want to see "</body></$html>" at the end of $html:
        if(strpos($s, "<body>") || strpos($s, "<body ")) {
            if (!strpos($s, "</body>")) {
                $p = strrpos($s, "</html>") - 1;
                $s = substr($s, 0, $p) . "</body>" . substr($s, $p, strlen($s) - $p);
            }
        }
        
        // want to see "</head><body>" in $html:  -- but not consider they may be in " "
        if(strpos($s, "<head>") || strpos($s, "<head ")) {
            if(!strpos($s, "</head>")) {
                $p = strrpos($s, "<body>")-1;
                if($p<=-1) $p = strrpos($s, "<body ")-1;
                $s = substr($s, 0, $p) . "</head>" . substr($s, $p, strlen($s)-$p);
            }
        }
    }
    return $s;
}

/*
* try to repair $html if it misses some tags;
* but we have to admit that sometime we cannot recover a syntax invalid $html,
* so we don't guarantee that the result is an error free $html in syntax
*/ 
function preprocess($s){ 
    $s = commentsRemove($s);  
    $s = aTitleRepair($s);  
    $s = HeadBodyHtmlRepair($s);  
    $s = MetaLinkInputImgRepair($s); 
    return $s;
}

/*
* backward search the first $tag whose $level equals $level in $tag[]
*/
function searchTag($tag, $level){
    $size=sizeof($tag);
    for($i=$size-1; $i>=0; $i--) {
        if($tag[$i]->level===$level) return $tag[$i]; 
    } 
    return null;
}

function printQ($state, $level){
		//if($state===1) echo "<br>";
		echo $state."  ".$level."<br>";
}

////////////////////////////////////////////////////////////////////
///                            main body                         ///          
////////////////////////////////////////////////////////////////////

/*
* parse HTML using FSA and save tags into $tag[]
*/
function parseHTML($s){
    $q0=0;
    $q1=1; $q2=2; $q3=3; $q4=4; $q5=5; $q6=6; $q7=7; $q8=8;
    $q11=11; $q21=21; $q31=31; $q41=41; $q71=71; $qq1=91; $qq2=92; $q211=211; $qq11=911;
    $q99=99; $qerr=-522; $qerr1=-5221; $qerr11=-52211; $qerr6=-5226; $qincomplete=-1430;

    $tag = array();
    $level=0;
    $state = $q0;
    $p=0; $len=strlen($s);
    global $MaxLengthOfTag;
    //$name="", $attr="", $value="";
		 
    while($p<$len) {
				//if($state===1)echo "<br>";	
        //printQ($state, $level); 
        switch($state) {
            case $q0:
                while($p<$len && ord($s[$p])<=32) $p++; //($s[$p]===' ' || $s[$p]===(char)(9))) $p++;  
								while($p<$len && $s[$p]!='<') $p++; 
                if($p===$len) { $state = $qincomplete; break; }
                if($s[$p] != '<') { echo "<br>from q0 <br>"; $state = $qerr11; break; }
                $state = $q1;
                $level++;
                break;
    
            case $q1:
                //print($s.substring($p-1,$p+12));
                while($p<$len && ord($s[$p])<=32) $p++;//&& $s[$p]===' ') $p++;
								if($s[$p]==='/') { $state = $q8; $level--; break; }  // "/head>..."
                if($p<$len && !isTagText($s[$p])) { $state = $q11; break; }
    
                $name = ""; 
                while($p<$len && isTagText($s[$p])) { $name .= $s[$p]; $p++; } 
                while($p<$len && ord($s[$p])<=32) $p++; 
                if($p===$len) break;
                if($s[$p] === '>' || ($p+1<$len && $s[$p] === '/' && $s[$p+1] === '>')) {
                    $state = $q2;
                    if($s[$p] === '/') $p++; // e.g., <br />
                }
                else if(isTagText($s[$p])) $state = $q3;
                else { $state = $q11; }
    
                if($state != $q11) {
                    $t1 = new Tag($name, $level);
                    array_push($tag, $t1);
                    if($state === $q3) $t1->attr = "" . $s[$p];
                }
                if($state === $q2 && ($name==="br" || $name==="hr")) $level--; // because these two tags start and also close here
    
                break;
    
            case $q2:
                //print("q2:" . (int)$s[$p] . "");
                while($p<$len && ord($s[$p])<=32) $p++; 
                if($p===$len) break;
                if($s[$p] === '<') { $state = $q1; }
                else {
                    $state = $q5;
                    searchTag($tag, $level)->value = "" . $s[$p];
                }
                //print("    $name=" . searchTag($tag, $level).$name);
                //if(searchTag($tag, $level).$attr !=null ) print("    $attr=" . searchTag($tag, $level).$attr);
                //if(searchTag($tag, $level).$value !=null ) print("    $value=" .  searchTag($tag, $level).$value );
                if($state === $q1) $level++;
                break;
    
            case $q5:
                $value=""; 
                $name = searchTag($tag, $level)->name;
                // consider case like: <script> <sdsd> "<style> .swegsd@3.. <style>" dddsd "...</script>..." abbd </script>
                if($name==="script" || $name==="style") { // all contents bwtween <script>..</script> belongs to $value; same to <style> and </style>
                    while($p<$len && ord($s[$p])<=32) $p++; 
                    while($p<$len) {
                        $c = $s[$p];
                        if($c === '"') { $state = $qq2; break; }
                        // caution: $p+2+MaxLengthOfTag could be larger than strlen($s) for rare special case; consider </body></$html> always exist in $html so this wouldn't happen
                        else if($p+1<$len && $c === '<' && $s[$p+1] === '/' && containsTag(substr($s, $p+2, $MaxLengthOfTag))) { $state = $q6; $p++;  break; } // two chars "</" so $p++ here
                        else if($c === '<' && containsTag(substr($s, $p+1, $MaxLengthOfTag))) {  $state = $q1; break;}
                        else $value .= $c;
                        $p++;
                    }
                }
                else {
                    while($p<$len && ord($s[$p])<=32) $p++; 
                    while($p<$len) {
                        $c = $s[$p];
                        if($c === '"') { $state = $qq2; break; }
                        else if($p+1<$len && $c === '<' && $s[$p+1] === '/') { $state = $q6; $p++;  break; } // two chars "</" so $p++ here
                        else if($c === '<') { $state = $q1; break;}
                        else $value .= $c;
                        $p++;
                    }
                }
                searchTag($tag, $level)->value .= trim($value);
                //print("$level=" . $level . ", $value=" .  searchTag($tag, $level).$value );
                if($state === $q1) $level++;

                //print($s.substring($p-2, $p+50));

                break;

            case $qq2:
                $value="\""; 
                while($p<$len && $s[$p] != '"') { $value .= $s[$p]; $p++; }
                if($p<$len) $state = $q5; //   $s[$p] === '"'

                searchTag($tag, $level)->value .= $value;  

                //print($s.substring($p-2, $p+50));
                break;

            case $q6:
                $name = "";
                while($p<$len && isTagText($s[$p])) { $name .= $s[$p]; $p++; }
                if(strlen($name)===0 || $p>=$len) { $state = $qincomplete; break; }
                $t = searchTag($tag, $level);
                if($t===null || !$t->name===$name) { $state = $qerr6; break; }

                if($s[$p] === '>') { $state = $q7; $level--; }
                else $state = $qerr;

                break;

            case $q8:
                $name=""; 
                while($p<$len && isTagText($s[$p])) { $name .= $s[$p]; $p++; } 
                if(strlen($name)===0 || $p>=$len) { $state = $qincomplete; break; }
 
                if($s[$p] === '>') { $state = $q7; }//{ $state = $q7; $level--;}
                else $state = $qerr;

                $level--; // because when we see '<' we always $level++ even for '</...>'

                break;

            case $q7:
                while($p<$len && ord($s[$p])<=32) $p++; 
                if($p===$len) { $state = $q99; break; } // eof
                if($s[$p] === '<') { $state = $q1; $level++; }
                //else $state = $qerr;
                else { searchTag($tag, $level)->value .= $s[$p]; $state = $q5; }// $value
                break;

            case $q3:
                $attr = "";
                while($p<$len) {
                    $c = $s[$p];
                    if($c === '=')  { $state = $q4; $attr .= $c; break; }
                    else if(isText($c)) { $attr .= $c; $p++; }
                    else { $state = $qerr1; break; }
                    }
                if($state != $qerr1) searchTag($tag, $level)->attr .= $attr;
                //print("q3: $attr=" . $attr);
                break;
            /*
            * 	<head>
                <meta charset="utf-8"/>
                <title>$jsTree test</title>
                <link rel="style"   href="dist/themes"  />
            </head>
            */
            case $q4:
                //while($p<$len && $s[$p]===' ') $p++;
                $attr = ""; 
                while($p<$len) { // look forward when we see '>' or "/>" -- we don't know it is followed by the $value or another $tag
                    $c = $s[$p];
                    if(isText($c)) $attr .= $c;
                    else if($c === '"') { $state = $qq1; $attr .= $c; break; }
                    else if($c===' '){
                        if($p+1<$len && isTagText($s[$p+1])) {$state = $q3; $attr .= $c; break;}
                    }
                    else if($c==='/') { $state = $q7; $level--; $p++; break;} // ... />.. <
                    else if($c==='>') {
                        if(isAheadValue($s, $p)) { $state = $q5; break; } // ... > .. abc
                        else { $state = $q7; break; }// ... >.. <
                    }
                    $p++;
                }
                //if($p===$len && $state === $q4) $state = $qincomplete; // -- no need judge this here; judge it at the end of this while() loop.
                searchTag($tag, $level)->attr .= $attr;
                //if($state === $q7) $level--;
                //print("q4: $attr=" . $attr);
                break;

            case $qq1:
                $attr = "";
                while($p<$len && $s[$p] != '"') { $attr .= $s[$p]; $p++; }
                if($p<$len) { $state = $q4; $attr .= $s[$p]; searchTag($tag, $level)->attr .= $attr;}
                //else $state = $qincomplete; // no transmission
                //print("qq1: $attr=" . $attr);
                break;

            case $q11:
                //print($s.substring($p-5, $p+5));
                ////if(isTagText($s[$p])) { $tag->add(new Tag("" . $s[$p], $level));  $state = $q21; }
                //else if($p+1<$len && $s[$p] === '-' && $s[$p+1] === '-') { $state = $q211; $p++; }
                //else $state= $qerr1;
                ////else { $state = $q211; }
                $state = $q211;
                break;

            case $q21:
                $name = "";
                while($p<$len && isTagText($s[$p])) { $name .= $s[$p]; $p++; }
                if(strlen($name)>0) searchTag($tag, $level)->name .= $name;
                if($p<$len && ord($s[$p])<=32) $state = $q31;                
								else $state = $qerr1;
                break;

            case $q31:
                while($p<$len && ord($s[$p])<=32) $p++; 
                $attr = "";
                while($p<$len && isTagText($s[$p])) { $attr += $s[$p]; $p++; }
                if(strlen($attr)>0) {
                    while($p<$len && ord($s[$p])<=32) $p++;  
                    if($p<$len && $s[$p]==='=') { $state = $q41; $attr .= "="; }
                    else $state = $qerr1;
                }
                else $state = $qerr1;
                if($state != $qerr1) searchTag($tag, $level)->attr .= $attr;
                break;

            case $q41:
                $attr="";
                while($p<$len) {
                    $c = $s[$p];
                    if($c===' ') { $state = $q31; $attr .= $c; break; }
                    else if($c==='>') { $state = $q71; $level--; break;}
                    else if($c==='"') { $state = $qq11; $attr .= $c; break; }
                    else $attr .= $c;
                    $p++;
                }
                searchTag($tag, $level)->attr .= $attr;
                break;

            case $qq11:
                $attr = "";
                while($p<$len && $s[$p] != '"') { $attr .= $s[$p];  $p++; }
                if($p<$len) { $state = $q41;  $attr .= $s[$p]; searchTag($tag, $level)->attr .= $attr;}
                //else $state = $qincomplete; // no transmission
                break;

            case $q211:
                //print($s.substring($p-2,$p+6));
                if($s[$p]==='-' && $s[$p+1]==='-'){
                    $p+=2;
                    while($p+2<$len && !($s[$p] ==='-' && $s[$p+1] ==='-' && $s[$p+2] ==='>')) $p++;
                    $p+=2;
                    $state = $q71; $level--;
                }
                else{
                    $p1=$p-1;
                    while($p<$len && $s[$p] !='>') $p++;
                    if($p<$len) { $t1 = new Tag(substr($s, $p1, $p-$p1), $level); $t1->hasChild=false; array_push($tag, $t1); $state = $q71; $level--; } // && $s[$p]==='>'
                }

                break;

            case $q71:
                while($p<$len && ord($s[$p])<=32) $p++; 
                if($p<$len && $s[$p]==='<') { $state = $q1; $level++; }
                else { $state = $qerr11; } 
                break;

            default: { $p=$len; break; }
        }//switch()

        $p++;
    }

    //printQ($state, $level);

    if($state === $q7 || $state === $q71) $state = $q99;
    if($state>=0 && $state != $q99) $state = $qincomplete;
    ///print("<br>final state=" . $state . ", level=" . $level."<br>");

    if($state === $q99 && $level===0) print("HTML is parsed successfully.<br>");
	else { print("Probably HTML syntax error (e.g., due to tags unclosed or tags missed)."."<br>"); return null; }
    return $tag;
}//parse()

/*
* src="/icons/ubuntu-logo.png" 	  alt="Ubuntu Logo" class="floating element"
* -->
* a[0]="src='\/icons\/ubuntu-logo.png'";
* a[1]="alt='Ubuntu Logo'";
* a[2]="class='floating element'";
*/
function parseAttrs($s){
    $attrs = array();
    $s = trim($s);//.replace("/", "\\/");
    $p=0; $p1=0; $len=strlen($s);
    //print("$attr=(" . $s . ")");
    $quoted = false;
    while($p<$len){
        if($s[$p]==='"') $quoted = !$quoted; 
				if(ord($s[$p])===32 && !$quoted) {
            array_push($attrs, str_replace('"', '', substr($s, $p1, $p-$p1)));
            //print(" find an $attri=(" . $s.substring($p1, $p) . ")");
            while($p<$len && ord($s[$p])<=32) $p++;
            $p1 = $p;
        }
        else {
            $p++;
            if($p===$len){
                //print(" find an $attri=(" . $s.substring($p1, $p) . ")");
                array_push($attrs, str_replace('"', '', substr($s, $p1, $p-$p1)));
            }
        } 
    }

    return $attrs;
}

/*
* extract the $jsTree data from $tag[]
* take $attr and $value as its children, $level++
* set its $hasChild to true if the $tag has child
*/
function parseTag($tag){
    // chanage '/' to '\/' in $attr
    $newtag = array();
    //$attri.replace("/", "\\/");
    foreach($tag as $t){  
        $level = $t->level;
        array_push($newtag, $t);
        if($t->attr != null){//} && !isEmpty($t->attr)) {
            $attrs = parseAttrs($t->attr);  ////
            foreach($attrs as $a) {
                ///a = a.replace("/", "\\/");
                $a = str_replace("'", '~', $a);//??  e.g., Unbuntu'$s apache -- '$s will interrupt interpretition
                $t1 = new Tag($a, $level+1); // note: the $attr of $t is the $name of $t1
                //$t1->hasChild = false; // leaf node
                ///print($t1->name."<br>");
                array_push($newtag, $t1);
            }
        }
        if($t->value != null){//} && !$t->value.isEmpty()) {
            ///$t->value = "$value=\"" . $t->value.replace("/", "\\/") . "\"";
            //$t->value = "value=\"" . $t->value . "\"";
						$t->value = 'value="' . $t->value . '"';
            $t->value = str_replace("'", '~', $t->value); //?? -- "'" cannot be parsed by JSON jstree
            $t1 = new Tag($t->value, $level+1); // note: the $value of $t is the $name of $t1
            //$t1->hasChild = false; // leaf node
            ////print("  $t->value=" . $t1->name);
            array_push($newtag, $t1);
        }
     }

    $tag = $newtag;
    $size = sizeof($tag);
    for($i=0; $i<$size-1; $i++) {
        $tag[$i]->hasChild = $tag[$i+1]->level > $tag[$i]->level;
    }
    if(sizeof($tag)>=1) $tag[sizeof($tag)-1]->hasChild = false;

    $js = "";
    for($i=0; $i<$size; $i++) {
        $t = $tag[$i];
        $level = $t->level;
        $jsTag = "";  // portion of js data parsed from this $t
        ///$jsTag .= nSpaces($level);
        $name = $t->name;
        if(strlen($name)>60) $name = substr($name, 0, 30) . " ... " . substr($name, strlen($name)-30, 30);
        $jsTag .= "{text:'" .  $name . "'";
        if($t->hasChild) $jsTag .= ",children:[";//\n";
        else {
            $jsTag .= "}";
            if($i<$size-1){
                $difflevel = $level - $tag[$i+1]->level;
                if($difflevel>0) {
                    ///$jsTag .= " ";//\n";
                    $jsTag .= nBrackets($difflevel, $level - 1) . ",";//\n";
                }
                else $jsTag  .= ",";//\n";
            }
            else if($level-1>0) { 
				///$jsTag .= " "; 
				$jsTag .= nBrackets($level-1, $level-1); }//// ? $level-1
        }
        $js .= $jsTag;
    }

    return $js;
}

////////////////////////////////////////////////////////////////////  
function HTML2jsTreeData($html){ 
    $tag = parseHTML($html);
    $jsTree = parseTag($tag);   
		$res = "";
    for($i=0; $i<strlen($jsTree); $i++){
			if(ord($jsTree[$i]) >= 32) $res .= $jsTree[$i];
		} 
    return  $res;
}

function buildJSTree($jsTreeData){
	$htmlHeadBody = "<!DOCTYPE html><html><head><meta charset='utf-8'/><script src='http://code.jquery.com/jquery-1.9.1.js'></script> <script src='jstree.min.js'></script> <link rel='stylesheet' href='style.min.css'/> <style> .divbox {  background:#FFF; border: 2px solid #ccc;  -moz-border-radius: 15px;  border-radius: 15px; } </style>  </head> <body> <div id='div2' class='divbox' title='jstree'> <div id='tree'>tree</div>  </div> ";
		 
	$script = "<script> var json =[". $jsTreeData ."]; $('#tree').jstree({'core' : {'data' : json} }); </script> </body> </html>";

	$jsTreeScript = $htmlHeadBody . $script;
	
	echo "<br>Preview of JSON Tree of this webpage:";
	echo "<br>".$jsTreeScript."<br>";

	return $jsTreeData;
}

function writeJsTreeData($jsTree, $jsTreeFile){
    $myFile = fopen($jsTreeFile, "w") or die("Unable to open file!");
    fwrite($myFile, $jsTree);
    fclose($myFile);
    print "jsTree data has been saved in " . $jsTreeFile."<br><br>";
}


function main()  {  
	/*	  
	  $filename = "/var/www/html/google_Ubun.html"; 
    $html = readHTMLFile($filename);  
	  echo "<br> HTML from file ".$filename."<br>";
	*/
 	
    $url = htmlspecialchars($_POST['url']);
    $html = file_get_contents($url);  ///// ----- ????? read only partial contents from www.google.com; last <script> ... </script> lost. why?????

    if($html===null || strlen($html)===0) {
			echo "<br>HTML is null.<br>";		
			return; 		 
		}
		else echo "URL=".$url."<br>"; 
    $html = preprocess($html);  
		if($html!=null)	$jsTreeData = HTML2jsTreeData($html); 

    if($jsTreeData!=null) { 
		buildJSTree($jsTreeData); 
		writejsTreeData($jsTreeData, "/home/adamant/JsonTreeData.json");
		echo "<br>" . "JSON tree data:<br><br>" . $jsTreeData."<br><br>"; //*/
	}   
}
  
main(); // build a json tree view 

?>

</body>
</html>

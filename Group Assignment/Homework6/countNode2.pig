lines = LOAD 'web-Google.txt' AS (node1:chararray, node2: chararray);
nodes = FOREACH lines GENERATE FLATTEN(TOKENIZE(node1)) as keynode,  FLATTEN(TOKENIZE(node2)) as linknode;
nodes2 = FOREACH lines GENERATE FLATTEN(TOKENIZE(node1)) as keynode2,  FLATTEN(TOKENIZE(node2)) as linknode2;

joinResult = JOIN nodes BY linknode, nodes2 BY keynode2 ;
joinFilter = FOREACH joinResult GENERATE keynode, linknode2;
unionResult = UNION  nodes, joinFilter;
groupUnion = GROUP unionResult BY $0;

filterResult = FOREACH groupUnion GENERATE group, unionResult.$1;

store filterResult into 'my_WC_pig_out5';
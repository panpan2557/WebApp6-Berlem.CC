lines = LOAD 'web-Google.txt' AS (node1:chararray, node2: chararray);
nodes = FOREACH lines GENERATE FLATTEN(TOKENIZE(node1)) as node;
grouped = GROUP nodes BY node;

nodeCount = FOREACH grouped GENERATE group, COUNT(nodes);
store nodeCount into 'my_WC_pig_out4';
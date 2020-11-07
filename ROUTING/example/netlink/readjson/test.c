#include <stdio.h>
#include <jansson.h>
#include <string.h>

// 実行時  -ljansson が必要 janssonがインストールされてない場合 sudo apt-get install libjansson-dev

int main()
{
    json_t* read_json_ob;
    json_error_t error;

    char ipadress[100];
    char routerid[100];

    read_json_ob = json_load_file("./test.json", 0, &error);
    if ( read_json_ob == NULL )
    {
        printf("NULL\n");
        return 1;
    }

    strcpy(routerid, json_string_value(json_object_get(read_json_ob, "router-id")));
    printf("%s \n", routerid);

    json_t* read_json_ob_neighbors;
    read_json_ob_neighbors = json_object_get(read_json_ob, "neighbors");
    if (read_json_ob_neighbors == NULL) {
        printf("NULL \n");
        return 0;;
    }

    int index;
    json_t *read_json_ob_ipadress;
    json_array_foreach(read_json_ob_neighbors, index, read_json_ob_ipadress) {
    stpcpy(ipadress, json_string_value(json_object_get(read_json_ob_ipadress, "address")));
    printf("%s \n", ipadress);
    };


    return 0; 

}
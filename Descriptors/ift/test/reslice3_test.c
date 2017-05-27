#include "ift.h"

int main(int argc, char **argv) {

  Scene *scn = NULL, *tmp = NULL;
  char rotation;
  char filename[100];

  if (argc != 3) {
    printf("usage: %s <base_name> <rotation>[CW(0), CCW(1)]\n", argv[0]);
    return (-1);
  }

  sprintf(filename, "%s.scn", argv[1]);
  scn = ReadScene(filename);

  rotation = (char)atoi(argv[2]);

  /* ResliceX */
  tmp = CopyScene(scn);
  ResliceX(&tmp, rotation);
  sprintf(filename, "%s-resliceX.scn", argv[1]);
  WriteScene(tmp, filename);
  DestroyScene(&tmp);  

  /* ResliceY */
  tmp = CopyScene(scn);
  ResliceY(&tmp, rotation);
  sprintf(filename, "%s-resliceY.scn", argv[1]);
  WriteScene(tmp, filename);
  DestroyScene(&tmp);
  
  /* ResliceZ */
  tmp = CopyScene(scn);
  ResliceZ(&tmp, rotation);
  sprintf(filename, "%s-resliceZ.scn", argv[1]);
  WriteScene(tmp, filename);
  DestroyScene(&tmp);  

  /* ResliceXY */
  tmp = CopyScene(scn);
  ResliceX(&tmp, rotation);
  ResliceY(&tmp, rotation);
  sprintf(filename, "%s-resliceXY.scn", argv[1]);
  WriteScene(tmp, filename);
  DestroyScene(&tmp);  

  /* ResliceXZ */
  tmp = CopyScene(scn);
  ResliceX(&tmp, rotation);
  ResliceZ(&tmp, rotation);
  sprintf(filename, "%s-resliceXZ.scn", argv[1]);
  WriteScene(tmp, filename);
  DestroyScene(&tmp);  

  /* ResliceYZ */
  tmp = CopyScene(scn);
  ResliceY(&tmp, rotation);
  ResliceZ(&tmp, rotation);
  sprintf(filename, "%s-resliceYZ.scn", argv[1]);
  WriteScene(tmp, filename);
  DestroyScene(&tmp);  

  return (0);

}

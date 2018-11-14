
/*--------------------------------------------------------------------------*/
/*
 * MP3 player DEMO - MPEG audio decoder demo program
 * Copyright (C) 2006 MXP semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY and WITHOUT ANY SUPPORT without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: demo.c 1.0 2006/10/16 09:41:32 rob Exp $
 */
/*--------------------------------------------------------------------------*/
# include <stdio.h>
# include <efs.h>
# include <ls.h>
# include <string.h>

# include <targets/lpc2148.h>
# include "lpc_io.h"
# include "mad.h"
# include "debug.h"
# include "midmad.h"

EmbeddedFileSystem  efs;
EmbeddedFile        file;
DirList             list;
unsigned char       file_name[13];
unsigned int        size;

void format_file_name(unsigned char *dest, unsigned char *src)
{
  unsigned char i,k;

    for (i=7; i>0 && (src[i] == 0x20); i--);

    strncpy(dest, src, i+1);
    dest[i+1] = '.';

    for (k=2; k>0 && (src[8+k] == 0x20); k--);

    strncpy(&dest[i+2], &src[8], k+1);
    dest[i+5] = '\0';
}

int main()
{
  init_IO();

  if(efs_init(&efs,"\\")!=0)
  {
    DBG((TXT("Could not open filesystem.\n")));
    return(-1);
  }

  if (ls_openDir(&list, &(efs.myFs), "/") != 0)
  {
    DBG((TXT("Could not open the selected directory.\n")));
    return(-2);
  }

  while (ls_getNext(&list) == 0)
  {
    if  ( (list.currentEntry.FileName[8] == 'M') &&
	  (list.currentEntry.FileName[9] == 'P') &&
          (list.currentEntry.FileName[10] == '3') )
      {
        DBG((TXT("Filename: %.11s (%li bytes)\n"), list.currentEntry.FileName,
                                                   list.currentEntry.FileSize));

        format_file_name(file_name, list.currentEntry.FileName);

	if(file_fopen(&file,&efs.myFs,file_name,'r')==0)
	{
          mp3_play(&file);
          file_fclose(&file);
	  TOGGLE_LIVE_LED1();
	}
	else
          DBG((TXT("Could not open file.\n")));

    } /* if */
  } /* while */

  fs_umount(&(efs.myFs));

  return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2014-2019, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-666778
// 
// All rights reserved.
// 
// This file is part of Conduit. 
// 
// For details, see: http://software.llnl.gov/conduit/.
// 
// Please also read conduit/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
///
/// file: t_libyaml_smoke.cpp
///
//-----------------------------------------------------------------------------

#include "yaml.h"
#include "gtest/gtest.h"

//-----------------------------------------------------------------------------
TEST(libyaml_smoke, basic_event_use)
{
    yaml_parser_t parser;
    yaml_event_t  event;

    // Initialize parser
    EXPECT_TRUE(yaml_parser_initialize(&parser));

    const char yaml_str[] = " hello : world\n line: 2\n";

    // set input
    yaml_parser_set_input_string(&parser,
                                 (const unsigned char*)yaml_str,
                                 strlen(yaml_str));

    bool ok = true;
    bool found_mapping = false;
    bool found_scalar   = false;
    
    bool found_hello = false;
    bool found_world = false;

    while(ok)
    {
        EXPECT_TRUE(yaml_parser_parse(&parser, &event));

    
        switch(event.type)
        { 
            case YAML_MAPPING_START_EVENT:
            {
                found_mapping = true;
                break;
            }
            case YAML_SCALAR_EVENT: 
            {
                found_scalar = true;
                std::string val((const char*)event.data.scalar.value);
                std::cout << val << std::endl;
                if(val == "hello")
                {
                    found_hello = true;
                }
                else if( val == "world")
                {
                    found_world = true;
                }
                break;
            }
            default:
                break;
        }

        ok = event.type != YAML_STREAM_END_EVENT;
        yaml_event_delete(&event);
    }

    EXPECT_TRUE(found_mapping);
    EXPECT_TRUE(found_scalar);
    EXPECT_TRUE(found_hello);
    EXPECT_TRUE(found_world);


    // cleanup parser
    yaml_parser_delete(&parser);
}

//-----------------------------------------------------------------------------
TEST(libyaml_smoke, basic_document_use)
{
    yaml_parser_t parser;
    yaml_document_t document;

    const char yaml_str[] = " hello : world\n line: 2\n";

    // Initialize parser
    EXPECT_TRUE(yaml_parser_initialize(&parser));

    // set input
    yaml_parser_set_input_string(&parser,
                                 (const unsigned char*)yaml_str,
                                 strlen(yaml_str));

    // construct document
    EXPECT_TRUE(yaml_parser_load(&parser, &document));
    yaml_node_t *node = yaml_document_get_root_node(&document);

    EXPECT_TRUE(node != NULL);

    // for this example, we we know the root is a mapping node
    EXPECT_TRUE( node->type == YAML_MAPPING_NODE );

    yaml_node_pair_t *pair = node->data.mapping.pairs.start;
    EXPECT_TRUE(pair != NULL);

    yaml_node_t *key = yaml_document_get_node(&document, pair->key);
    yaml_node_t *val = yaml_document_get_node(&document, pair->value);

    EXPECT_TRUE(key != NULL);
    EXPECT_TRUE(val != NULL);
    
    std::string key_name((const char*)key->data.scalar.value);
    std::string val_str((const char*)val->data.scalar.value);

    std::cout << key_name << ": " << val_str << std::endl;

    EXPECT_EQ(key_name,"hello");
    EXPECT_EQ(val_str,"world");

    // cleanup doc
    yaml_document_delete(&document);

    // cleanup parser
    yaml_parser_delete(&parser);
}


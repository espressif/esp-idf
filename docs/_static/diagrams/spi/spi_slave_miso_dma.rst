.. This is the source code to generate figure by https://wavedrom.com/.
.. We can use the sphinx plugin sphinxcontrib-wavedrom to generate the figure and put it in the html doc (by include this source code), but it is not supported by the pdf. Currently we generate the figure manually and include the stataic figure in the doc.

.. wavedrom::

    { signal: [
      { name:'CS', 		wave:'10........'},
      { name: 'SCLK',	wave: '0.10101010',  node: '...ab'},
      { name: 'MISO (normal)', 	wave: 'x3.3.3.3.3',  node: '...c.d', data: ['7','6','5','4','3'], phase:-0.8},
      { name: 'SCLK',	wave: '0.10101010',  node: '..e.f'},
      { name: 'MISO (DMA)', 	wave: 'x33.3.3.3',  node: '..g.h', data: ['7','6','5','4','3','2'], phase:-0.8},
      ],
      edge: [
        'a|->c delay',
        'b-|->c setup',
        'b-|->d hold',
      	'e|->g delay',
      	'f|->g setup',
      	'f-|>h hold',
      ],
     config:{hscale: 2}
    }


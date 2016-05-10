from elasticsearch import Elasticsearch
import urllib.request,json,sys

"""
Daniel Correa - Cristina Extremera - Gerardo Bernal
Entrega 3, Elasticsearch, RIBW

Implementación de busqueda de restaurantes consultando el open data de Cáceres, indexándolo en Elasticsearch con Python. Busquedas por id, nombre y todos.
"""

def consultar_opendata():
	"""
	Permite consultar el open data de Cáceres de restaurantes y recibirlo en formato JSON
	"""
	response = urllib.request.urlopen("http://opendata.caceres.es/GetData/GetData?dataset=om:Restaurante&format=json")
	data = response.read().decode(response.info().get_param('charset') or 'utf-8')
	data_json = json.loads(data)
	return data_json['results']['bindings']

def indexar_informacion(json_data,es):
	"""
	Permite indexar la información obtenida del open data en la tabla restaurantes
	"""
	i=0
	for restaurante in json_data:
		res = es.index(index="restaurantes", doc_type='restaurante', id=i, body=restaurante)
		i+=1
def imprimir_restaurante(res): 
	"""
	Permite imprimir la información obtenida de la busqueda en formato entendible
	"""
	print("Nombre: " + str(res['_source']['rdfs_label']['value']))
	print("Teléfono: " + str(res['_source']['schema_telephone']['value']))
	print("Dirección: " + str(res['_source']['schema_address_streetAddress']['value']))
def buscador():
	"""
	Se encarga de gestionar la comunicación entre elasticsearch y usuario
	"""
	#Se iniciliaza la informacion del buscador
	es = Elasticsearch()
	json_data = consultar_opendata()
	indexar_informacion(json_data,es)

	print("Buscador de restaurantes!")
	opcion = 0
	while opcion!=4:
		print("Menu: \n - 1 para buscar por id \n - 2 para buscar por nombre de restaurante \n - 3 para mostrar la lista completa de restaurantes \n - 4 para finalizar")
		try:
			opcion = int(input("Digite la opción que desea: "))
		except:
			opcion = 0
		if opcion == 1:
			opcion = input("Ingresa el id del restaurante: ")
			res = es.get(index="restaurantes", doc_type='restaurante', id=int(opcion))
			print("..................")
			imprimir_restaurante(res)
			print("..................")
		elif opcion == 2: 
			opcion = input("Ingresa el nombre del restaurante: ")
			res = es.search(index="restaurantes", body={"query":{"match": {"rdfs_label.value": str(opcion)}}})
			print("NOTA: ESTA CONSULTA REALIZA LA BÚSQUEDA DE TODAS LAS PALABRAS INTRODUCIDAS USANDO EL OPERADOR OR")
			print("..................")
			for hit in res['hits']['hits']:
				imprimir_restaurante(hit)
				print("..................")
		elif opcion == 3:
			res = es.search(index="restaurantes", body={"query": {"match_all": {}}})
			print("TOTAL ENCONTRADOS: "+str(res['hits']['total']))
			print("NOTA: ESTA CONSULTA MUESTRA SOLO LOS 10 PRIMEROS RESULTADOS")
			print("..................")
			for hit in res['hits']['hits']:
				imprimir_restaurante(hit)
				print("..................")
		elif opcion == 4:
			sys.exit()
		else:
			print("Opción inválida")

buscador()
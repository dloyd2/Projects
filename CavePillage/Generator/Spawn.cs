using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//ensure that enemy prefabs are ordered lowest cost -> highest, else list order issues will occur
public class Spawn : MonoBehaviour
{
    int totalValue;
    EnemyType toSpawn;
    int valueToSpawn =0;
    static public EnemyType typeToSpawn;
    GameObject spawned;
    float waitTime = 3;
    float timer = 0;
    int numEnemies = (int)EnemyType.NUMENEMIES;
    public void SetValue(int newValue)
    {
        totalValue = newValue;
    }
    
    void Awake()
    {
        Debug.Log(numEnemies + " enemies to choose from");
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;
        if(totalValue > 0 && timer >= waitTime)
        {
                Debug.Log("TotalValue currently " + totalValue);
                toSpawn = (EnemyType)Mathf.Floor(Random.Range(0, numEnemies));
                Debug.Log("toSpawn randomly selected " + toSpawn);
                if(!string.Equals(Auxillary.FindType(toSpawn), "not found"))
                {
                    valueToSpawn = Auxillary.enemyValues[toSpawn];
                }
                else
                {
                    toSpawn = (EnemyType)0;
                }
                Debug.Log("ValueToSpawn chosen " + Auxillary.enemyValues[toSpawn]);
            while(valueToSpawn > totalValue)
            {
            //find closest enemy that can be spawned
                toSpawn --;
                Debug.Log("value is to high, toSpawn is now " + toSpawn);
                valueToSpawn = Auxillary.enemyValues[toSpawn];
            }
            
            Debug.Log("toSpawn at end " + toSpawn);
            Debug.Log("ValueToSpawn at end " + valueToSpawn);
            spawned = Instantiate(Auxillary.enemies[toSpawn]) as GameObject;
            spawned.transform.position = transform.position;
            totalValue -= valueToSpawn;
            timer = 0;
        }
        waitTime = Random.Range(2, 5);
            
        
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpawnManager : MonoBehaviour
{
    GameObject[] generators;
    static public int numEnemies = 0;
    int spawnValue = 3;
    int spawnValueForGen = 0;
    float remainingSpawnValue;
    int numGens;
    float maxRoundTimer = 3f;
    float roundTimer = 0f;
    
    // Start is called before the first frame update
    
    static public void ChangeNumEnemies(int changeValue)
    {
        numEnemies += changeValue;
        Debug.Log("numEnemies changed, it is now: " + numEnemies);
    }
    void SetSpawnValue()
    {
        spawnValue = (int)Mathf.Ceil(spawnValue*1.3f);
        remainingSpawnValue = spawnValue;
    }
    void Awake()
    {
        generators = GameObject.FindGameObjectsWithTag("Generator");
        numGens = generators.Length;
    }
    void Start()
    {
        numEnemies = 0;
    }
    // Update is called once per frame
    void Update()
    {
         if (numEnemies == 0)
         {
             roundTimer += Time.deltaTime;
             if(roundTimer >= maxRoundTimer)
             {
                 Debug.Log("*****NEW SPAWN WAVE*****\nspawning " + spawnValue + " enemies");
                for(int i = 0; i < numGens; i++)
                {
                    spawnValueForGen = i == numGens-1 ? (int)remainingSpawnValue : (int)Mathf.Ceil(Random.Range(0, remainingSpawnValue/(float)spawnValue) *spawnValue);
                    Debug.Log(spawnValueForGen + " SpawnValue set for generator");
                    remainingSpawnValue -= spawnValueForGen;
                    generators[i].GetComponent<Spawn>().SetValue(spawnValueForGen);
                }
                SetSpawnValue();
                roundTimer = 0f;
             }
         }
    }
}

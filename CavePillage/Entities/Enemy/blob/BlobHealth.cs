using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlobHealth : Enemy
{
    public override void Awake()
    {
        base.Awake();
        startingHealth = 2;
        defense = 0;
        type = EnemyType.BLOB;
        remainingHealth = startingHealth;
        timeBetweenAttacks = 1f;
        attackDamage = 1;
        chanceToDrop = 20f;
        score = 2;
        SpawnManager.ChangeNumEnemies(1);
    }
    // Start is called before the first frame update
    void Start()
    {
        
    }
    
    // Update is called once per frame
    void Update()
    {
        CheckStatus();
    }
    
    
}
